#include "engine/graphics/graphics.h"

#include "engine/assets/library.h"
#include "engine/engine.h"
#include "engine/graphics/descriptor.h"
#include "engine/graphics/types.h"
#include "engine/graphics/util.h"
#include "engine/graphics/vulkan.h"
#include "engine/logger.h"
#include "engine/rwlock.h"
// clang-format off
#include <volk/volk.h>
// clang-format on
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <Tracy/Tracy.hpp>
#include <Tracy/TracyVulkan.hpp>
#include <Tracy/common/TracySystem.hpp>
#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <fmt/format.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_vulkan.h>
#include <span>
#include <type_traits>
#include <utility>
#include <vector>
#include <vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>

void ENGINE_NS::GraphicsEngine::initialise() {
    FrameMarkStart(StaticNames::GraphicsInit);
    auto logger = ENGINE_NS::g_ENGINE->logger.get(ENGINE_NS::LogNamespaces::GRAPHICS);
    logger.get().info("Creating graphics engine");

    logger.get().info("Initialising SDL");
    SDL_Init(SDL_INIT_VIDEO);
    {
        ZoneScoped;
        SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN);

        window_                      = SDL_CreateWindow("Vulkan Engine", window_extent_.x, window_extent_.y, window_flags);
    }

    logger.get().info("Initialising Vulkan");
    init_vulkan_();

    logger.get().info("Initialising render thread");
    {
        ZoneScoped;
        running_.store(true, std::memory_order_release);
        update_rate_   = std::chrono::milliseconds(8);
        render_thread_ = std::thread::thread(&ENGINE_NS::GraphicsEngine::draw_, this);
    }

    logger.get().info("Initialising upload thread");
    {
        ZoneScoped;
        upload_ready_.store(false, std::memory_order_release);
        upload_thread_ = std::thread::thread(&ENGINE_NS::GraphicsEngine::upload_, this);
    }

    init_immediates_();

    initialised_ = true;

    init_mesh_data_();

    FrameMarkEnd(StaticNames::GraphicsInit);
}

void ENGINE_NS::GraphicsEngine::draw() {
    FrameMarkStart(StaticNames::GraphicsDraw);
    frame_deletion_queue_.flush(device_, allocator_);

    {
        auto lock = imgui.write();
        ImGui::Render();
        auto draw_data = ImGui::GetDrawData();
        if (draw_data) {
            lock.get().latest_draw = draw_data;
        }
    }

    FrameMarkEnd(StaticNames::GraphicsDraw);
}

void ENGINE_NS::GraphicsEngine::cleanup() {
    FrameMarkStart(StaticNames::GraphicsDeinit);
    auto logger = ENGINE_NS::g_ENGINE->logger.get(ENGINE_NS::LogNamespaces::GRAPHICS);
    logger.get().info("Cleaning up graphics engine");

    logger.get().info("Stopping render thread");
    {
        ZoneScoped;
        if (this->running_.load(std::memory_order_acquire)) {
            this->running_.store(false, std::memory_order_release);
            this->render_thread_.join();
        }
    }
    logger.get().info("Stopping upload thread");
    {
        ZoneScoped;
        this->upload_thread_.join();
    }


    logger.get().info("Cleaning up Vulkan");
    if (device_.device != VK_NULL_HANDLE) {
        vkDeviceWaitIdle(device_.device);
        global_descriptor_allocator_.destroy();
        for (auto idx = 0; idx < graphics::FRAME_OVERLAP; idx++) {
            auto frame = frames_[idx].write();
            vkDestroySemaphore(device_.device, frame.get().swapchain_semaphore_, nullptr);
            vkDestroyFence(device_.device, frame.get().render_fence_, nullptr);

            if (frame.get().tracy_context_) {
                TracyVkDestroy(frame.get().tracy_context_);
            }
            vkDestroyCommandPool(device_.device, frame.get().command_pool, nullptr);

            frame.get().deletion_queue.flush(device_, allocator_);
        }
    }
    frame_deletion_queue_.flush(device_, allocator_);
    deletion_queue_.flush(device_, allocator_);
    upload_deletion_queue_.flush(device_, allocator_);
    swapchain_.cleanup();
    vmaDestroyAllocator(allocator_);
    device_.cleanup();
    surface_.cleanup();
    vulkan_instance_.cleanup();

    logger.get().info("Cleaning up SDL");
    {
        ZoneScoped;
        SDL_DestroyWindow(window_);
    }

    initialised_ = false;
    FrameMarkEnd(StaticNames::GraphicsDeinit);
}

auto ENGINE_NS::GraphicsEngine::current_frame() -> RwLock<graphics::FrameData>& {
    return frames_[frame_number_ % graphics::FRAME_OVERLAP];
}

auto ENGINE_NS::GraphicsEngine::allocate_buffer(std::size_t size, VkBufferUsageFlags flags, VmaMemoryUsage usage) -> BufferAllocation {
    VkBufferCreateInfo buffer_info = {};
    buffer_info.sType              = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size               = size;
    buffer_info.usage              = flags;

    VmaAllocationCreateInfo alloc_info{};
    alloc_info.usage = usage;
    alloc_info.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

    BufferAllocation buffer{};
    VK_CHECK(vmaCreateBuffer(allocator_, &buffer_info, &alloc_info, &buffer.buffer, &buffer.allocation, &buffer.info));
    return buffer;
}

auto ENGINE_NS::GraphicsEngine::destroy_buffer(BufferAllocation allocation) -> void {
    frame_deletion_queue_.push(allocation);
}

auto ENGINE_NS::GraphicsEngine::upload_mesh(std::span<std::uint32_t> indices, std::span<Vertex> vertices) -> std::future<GPUMeshBuffers> {
    graphics::MeshUpload upload{};
    upload.indices  = std::vector<std::uint32_t>(indices.begin(), indices.end());
    upload.vertices = std::vector<Vertex>(vertices.begin(), vertices.end());

    auto lock       = uploads_.write();
    auto& uploads   = lock.get();
    uploads.push_back(std::move(upload));
    auto future = uploads.back().promise.get_future();

    upload_ready_.store(true, std::memory_order_release);
    return future;
}

auto ENGINE_NS::GraphicsEngine::init_thread_(graphics::Thread thread) -> void {
    thread_init_mutex_.lock();
    thread_ids_.insert({std::this_thread::get_id(), thread});
    thread_init_mutex_.unlock();
}

auto ENGINE_NS::GraphicsEngine::init_vulkan_() -> void {
    auto logger = ENGINE_NS::g_ENGINE->logger.get(ENGINE_NS::LogNamespaces::GRAPHICS);
    ZoneScoped;
    logger.get().debug("Initialising Volk");
    VK_CHECK(volkInitialize());
    vulkan_instance_ = VulkanInstance::build()
                           .engine_name(ENGINE_NAME_STR)
                           .engine_version(Version(1, 0, 0))
                           .game_name("Shooter")
                           .game_version(Version(1, 0, 0))
                           .with_validation_layers(true)
                           .finish();

    logger.get().debug("Creating surface");
    surface_ = VulkanSurface(window_, vulkan_instance_);

    // vulkan 1.3 features
    VkPhysicalDeviceVulkan13Features features{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES};
    features.dynamicRendering = true;
    features.synchronization2 = true;

    // vulkan 1.2 features
    VkPhysicalDeviceVulkan12Features features12{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES};
    features12.bufferDeviceAddress = true;
    features12.descriptorIndexing  = true;

    VkPhysicalDeviceVulkan11Features features11{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES};
    features11.shaderDrawParameters = true;

    logger.get().debug("Picking physical device");
    physical_device_ = VulkanPhysicalDevice::choose(window_)
                           .set_minimum_vulkan_version(Version(1, 3, 0))
                           .set_required_features_11(features11)
                           .set_required_features_12(features12)
                           .set_required_features_13(features)
                           .with_extension(VK_KHR_SWAPCHAIN_EXTENSION_NAME)
                           .finish(vulkan_instance_);

    logger.get().debug("Initialising device");
    device_ =
        VulkanDevice::build()
            .request_queue("graphics", VulkanQueueType::GRAPHICS | VulkanQueueType::TRANSFER | VulkanQueueType::COMPUTE)
            .request_queue("transfer", VulkanQueueType::TRANSFER)
            .request_queue("imgui", VulkanQueueType::GRAPHICS | VulkanQueueType::TRANSFER | VulkanQueueType::COMPUTE)
            .request_queue(graphics::thread_immediate_name(graphics::Thread::MAIN), VulkanQueueType::TRANSFER | VulkanQueueType::COMPUTE)
            .request_queue(graphics::thread_immediate_name(graphics::Thread::UPLOAD), VulkanQueueType::TRANSFER | VulkanQueueType::COMPUTE)
            .request_queue(graphics::thread_immediate_name(graphics::Thread::DRAW), VulkanQueueType::TRANSFER | VulkanQueueType::COMPUTE)
            .finish(physical_device_);

    logger.get().debug("Initialising allocator");
    VmaVulkanFunctions vma_vulkan_funcs{};
    vma_vulkan_funcs.vkAllocateMemory                    = vkAllocateMemory;
    vma_vulkan_funcs.vkBindBufferMemory                  = vkBindBufferMemory;
    vma_vulkan_funcs.vkBindImageMemory                   = vkBindImageMemory;
    vma_vulkan_funcs.vkCreateBuffer                      = vkCreateBuffer;
    vma_vulkan_funcs.vkCreateImage                       = vkCreateImage;
    vma_vulkan_funcs.vkDestroyBuffer                     = vkDestroyBuffer;
    vma_vulkan_funcs.vkDestroyImage                      = vkDestroyImage;
    vma_vulkan_funcs.vkFlushMappedMemoryRanges           = vkFlushMappedMemoryRanges;
    vma_vulkan_funcs.vkFreeMemory                        = vkFreeMemory;
    vma_vulkan_funcs.vkGetBufferMemoryRequirements       = vkGetBufferMemoryRequirements;
    vma_vulkan_funcs.vkGetImageMemoryRequirements        = vkGetImageMemoryRequirements;
    vma_vulkan_funcs.vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties;
    vma_vulkan_funcs.vkGetPhysicalDeviceProperties       = vkGetPhysicalDeviceProperties;
    vma_vulkan_funcs.vkInvalidateMappedMemoryRanges      = vkInvalidateMappedMemoryRanges;
    vma_vulkan_funcs.vkMapMemory                         = vkMapMemory;
    vma_vulkan_funcs.vkUnmapMemory                       = vkUnmapMemory;
    vma_vulkan_funcs.vkCmdCopyBuffer                     = vkCmdCopyBuffer;
    vma_vulkan_funcs.vkGetInstanceProcAddr               = vkGetInstanceProcAddr;
    vma_vulkan_funcs.vkGetDeviceProcAddr                 = vkGetDeviceProcAddr;

    VmaAllocatorCreateInfo allocator_info{};
    allocator_info.physicalDevice   = physical_device_.device;
    allocator_info.device           = device_.device;
    allocator_info.instance         = vulkan_instance_.instance;
    allocator_info.flags            = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
    allocator_info.pVulkanFunctions = &vma_vulkan_funcs;
    vmaCreateAllocator(&allocator_info, &allocator_);

    logger.get().debug("Initialising swapchain");
    create_swapchain_();

    graphics_queue_           = device_.queues.at("graphics").get();
    transfer_queue_           = device_.queues.at("transfer").get();

    imgui.write().get().queue = device_.queues.at("imgui").get();

    VkCommandPoolCreateInfo pool_info =
        command_pool_create_info(device_.queues.at("graphics").family, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    VkSemaphoreCreateInfo semaphore_info = semaphore_create_info(0);
    VkFenceCreateInfo fence_info         = fence_create_info(VK_FENCE_CREATE_SIGNALED_BIT);

    logger.get().debug("Initialising sync structures");
    for (auto idx = 0; idx < graphics::FRAME_OVERLAP; idx++) {
        auto frame = frames_[idx].write();
        VK_CHECK(vkCreateCommandPool(device_.device, &pool_info, nullptr, &frame.get().command_pool));

        VkCommandBufferAllocateInfo buffer_alloc = command_buffer_allocate_info(frame.get().command_pool);
        VK_CHECK(vkAllocateCommandBuffers(device_.device, &buffer_alloc, &frame.get().main_command_buffer));
        frame.get().tracy_context_ =
            TracyVkContext(physical_device_.device, device_.device, device_.queues.at("graphics").get(), frame.get().main_command_buffer);

        VK_CHECK(vkCreateFence(device_.device, &fence_info, nullptr, &frame.get().render_fence_));
        VK_CHECK(vkCreateSemaphore(device_.device, &semaphore_info, nullptr, &frame.get().swapchain_semaphore_));
    }


    logger.get().debug("Initialising descriptors");
    init_descriptors_();
    logger.get().debug("Initialising pipelines");
    init_pipelines_();

    logger.get().debug("Initialising ImGui");
    init_imgui_();

    logger.get().debug("Initialising immediate");
    init_thread_(graphics::Thread::MAIN);
}

auto ENGINE_NS::GraphicsEngine::create_swapchain_() -> void {
    swapchain_ =
        VulkanSwapchain::build()
            .set_desired_format(VkSurfaceFormatKHR{.format = VK_FORMAT_B8G8R8A8_UNORM, .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR})
            .set_present_mode(VK_PRESENT_MODE_FIFO_KHR)
            .set_extent(::linalg::Vector2<unsigned int>(window_extent_.x, window_extent_.y))
            .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
            .add_image_usage_flags(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
            .finish(physical_device_, surface_, device_);

    VkExtent3D draw_extent = {static_cast<std::uint32_t>(window_extent_.x), static_cast<std::uint32_t>(window_extent_.y), 1};
    draw_image_.format     = VK_FORMAT_R16G16B16A16_SFLOAT;
    draw_image_.extent     = draw_extent;

    VkImageUsageFlags draw_image_usages{};
    draw_image_usages |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    draw_image_usages |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    draw_image_usages |= VK_IMAGE_USAGE_STORAGE_BIT;
    draw_image_usages |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    VkImageCreateInfo draw_img_info             = image_create_info(draw_image_.format, draw_image_usages, draw_extent);

    VmaAllocationCreateInfo draw_img_allocation = {};
    draw_img_allocation.usage                   = VMA_MEMORY_USAGE_GPU_ONLY;
    draw_img_allocation.requiredFlags           = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    vmaCreateImage(allocator_, &draw_img_info, &draw_img_allocation, &draw_image_.image, &draw_image_.allocation, nullptr);

    VkImageViewCreateInfo draw_image_view = image_view_create_info(draw_image_.format, draw_image_.image, VK_IMAGE_ASPECT_COLOR_BIT);

    VK_CHECK(vkCreateImageView(device_.device, &draw_image_view, nullptr, &draw_image_.view));

    deletion_queue_.push(draw_image_);
}

auto ENGINE_NS::GraphicsEngine::init_descriptors_() -> void {
    std::vector<DescriptorAllocator::PoolSizeRatio> sizes = {
      {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1}
    };
    global_descriptor_allocator_.init(device_, 10, sizes);
    draw_image_layout_ = VulkanDescriptorSetLayout::build()
                             .with_binding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE)
                             .build(device_, VK_SHADER_STAGE_COMPUTE_BIT, nullptr, 0);

    draw_image_descriptors_ = global_descriptor_allocator_.allocate(draw_image_layout_.layout);

    VkDescriptorImageInfo image_info{};
    image_info.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
    image_info.imageView   = draw_image_.view;

    VkWriteDescriptorSet draw_image_write{};
    draw_image_write.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    draw_image_write.pNext           = nullptr;

    draw_image_write.dstBinding      = 0;
    draw_image_write.dstSet          = draw_image_descriptors_;
    draw_image_write.descriptorCount = 1;
    draw_image_write.descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    draw_image_write.pImageInfo      = &image_info;

    vkUpdateDescriptorSets(device_.device, 1, &draw_image_write, 0, nullptr);
    deletion_queue_.push(draw_image_layout_);
}

auto ENGINE_NS::GraphicsEngine::init_imgui_() -> void {
    IMGUI_CHECKVERSION();
    auto& imgui_                                 = imgui.write().get();
    std::vector<VkDescriptorPoolSize> pool_sizes = {
      {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4 * IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE},
    };

    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType                      = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags                      = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets                    = 0;
    for (VkDescriptorPoolSize& pool_size : pool_sizes) {
        pool_info.maxSets += pool_size.descriptorCount;
    }
    pool_info.poolSizeCount = static_cast<std::uint32_t>(pool_sizes.size());
    pool_info.pPoolSizes    = pool_sizes.data();

    vkCreateDescriptorPool(device_.device, &pool_info, nullptr, &imgui_.descriptor_pool);

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui_ImplSDL3_InitForVulkan(window_);
    ImGui_ImplVulkan_InitInfo init_info                                            = {};
    init_info.Instance                                                             = vulkan_instance_.instance;
    init_info.PhysicalDevice                                                       = physical_device_.device;
    init_info.Device                                                               = device_.device;
    init_info.QueueFamily                                                          = device_.queues.at("imgui").family;
    init_info.Queue                                                                = imgui_.queue;
    init_info.PipelineCache                                                        = VK_NULL_HANDLE;
    init_info.DescriptorPool                                                       = imgui_.descriptor_pool;
    init_info.MinImageCount                                                        = 2;
    init_info.ImageCount                                                           = 2;
    init_info.Allocator                                                            = VK_NULL_HANDLE;
    init_info.UseDynamicRendering                                                  = true;
    init_info.CheckVkResultFn                                                      = check_vk_result;
    init_info.PipelineInfoMain.MSAASamples                                         = VK_SAMPLE_COUNT_1_BIT;
    init_info.PipelineInfoMain.PipelineRenderingCreateInfo.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    init_info.PipelineInfoMain.PipelineRenderingCreateInfo.colorAttachmentCount    = 1;
    init_info.PipelineInfoMain.PipelineRenderingCreateInfo.pColorAttachmentFormats = &swapchain_.format;

    ImGui_ImplVulkan_Init(&init_info);

    deletion_queue_.push(imgui_);
}

auto ENGINE_NS::GraphicsEngine::init_immediates_() -> void {
    for (auto& thread : {graphics::Thread::MAIN, graphics::Thread::DRAW, graphics::Thread::UPLOAD}) {
        auto& immediate              = immediates_.insert({thread, {}}).first.value();

        VkFenceCreateInfo fence_info = fence_create_info(VK_FENCE_CREATE_SIGNALED_BIT);
        VK_CHECK(vkCreateFence(device_.device, &fence_info, nullptr, &immediate.fence));

        auto& queue                       = device_.queues.at(graphics::thread_immediate_name(thread));
        immediate.queue                   = queue.get();

        VkCommandPoolCreateInfo pool_info = command_pool_create_info(queue.family, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

        vkCreateCommandPool(device_.device, &pool_info, nullptr, &immediate.command_pool);
        VkCommandBufferAllocateInfo command_alloc_info = command_buffer_allocate_info(immediate.command_pool);

        VK_CHECK(vkAllocateCommandBuffers(device_.device, &command_alloc_info, &immediate.command_buffer));

        immediate.tracy_context = TracyVkContext(physical_device_.device, device_.device, queue.get(), immediate.command_buffer);

        deletion_queue_.push(immediate);
    }
}

auto ENGINE_NS::GraphicsEngine::init_pipelines_() -> void {
    init_background_pipelines_();
    init_triangle_pipeline_();
    init_mesh_pipeline_();
}

auto ENGINE_NS::GraphicsEngine::init_background_pipelines_() -> void {
    VkPipelineLayoutCreateInfo compute_layout = pipeline_layout_create_info();
    compute_layout.pSetLayouts                = &draw_image_layout_.layout;
    compute_layout.setLayoutCount             = 1;

    VK_CHECK(vkCreatePipelineLayout(device_.device, &compute_layout, nullptr, &gradient_pipeline_.pipeline_layout));

    auto shader_result = asset::BytecodeShader::load_from_file("assets/shaders/engine/gradient.spv").compile(device_);
    if (!shader_result.has_value()) {
        crash(ErrorCode::CANNOT_READ_FILE, __LINE__, __func__, __FILE__);
        std::unreachable();
    }
    auto shader                                = shader_result.value();

    VkPipelineShaderStageCreateInfo stage_info = pipeline_shader_stage_create_info(VK_SHADER_STAGE_COMPUTE_BIT, shader.shader, "main");
    VkComputePipelineCreateInfo compute_create_info{};
    compute_create_info.sType  = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    compute_create_info.layout = gradient_pipeline_.pipeline_layout;
    compute_create_info.stage  = stage_info;

    VK_CHECK(vkCreateComputePipelines(device_.device, VK_NULL_HANDLE, 1, &compute_create_info, nullptr, &gradient_pipeline_.pipeline));

    vkDestroyShaderModule(device_.device, shader.shader, nullptr);

    deletion_queue_.push(gradient_pipeline_);
}

auto ENGINE_NS::GraphicsEngine::init_triangle_pipeline_() -> void {
    auto shader_result = asset::BytecodeShader::load_from_file("assets/shaders/engine/static_triangle.spv").compile(device_);
    if (!shader_result.has_value()) {
        crash(ErrorCode::CANNOT_READ_FILE, __LINE__, __func__, __FILE__);
        std::unreachable();
    }
    auto shader        = shader_result.value();

    triangle_pipeline_ = GraphicsPipeline::build()
                             .layout()
                             .finish()
                             .shaders(shader, shader)
                             .input_topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
                             .polygon_mode(VK_POLYGON_MODE_FILL)
                             .cull_mode(VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE)
                             .set_multisampling_none()
                             .disable_blending()
                             .disable_depthtest()
                             .color_attachment_format(draw_image_.format)
                             .depth_format(VK_FORMAT_UNDEFINED)
                             .finish(device_);
    vkDestroyShaderModule(device_.device, shader.shader, nullptr);

    deletion_queue_.push(triangle_pipeline_);
}

auto ENGINE_NS::GraphicsEngine::init_mesh_pipeline_() -> void {
    auto shader_result = asset::BytecodeShader::load_from_file("assets/shaders/engine/mesh_triangle.spv").compile(device_);
    if (!shader_result.has_value()) {
        crash(ErrorCode::CANNOT_READ_FILE, __LINE__, __func__, __FILE__);
        std::unreachable();
    }
    auto& shader = shader_result.value();

    VkPushConstantRange buffer_range{};
    buffer_range.offset     = 0;
    buffer_range.size       = sizeof(GPUDrawPushConstants);
    buffer_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    mesh_pipeline_          = GraphicsPipeline::build()
                         .layout()
                         .push_constant_range(buffer_range)
                         .finish()
                         .shaders(shader, shader)
                         .input_topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
                         .polygon_mode(VK_POLYGON_MODE_FILL)
                         .cull_mode(VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE)
                         .set_multisampling_none()
                         .disable_blending()
                         .disable_depthtest()
                         .color_attachment_format(draw_image_.format)
                         .depth_format(VK_FORMAT_UNDEFINED)
                         .finish(device_);
    vkDestroyShaderModule(device_.device, shader.shader, nullptr);

    deletion_queue_.push(mesh_pipeline_);
}

auto ENGINE_NS::GraphicsEngine::init_mesh_data_() -> void {
    std::array<Vertex, 4> rect_vertices{};
    rect_vertices[0].position = {0.5, -0.5, 0};
    rect_vertices[1].position = {0.5, 0.5, 0};
    rect_vertices[2].position = {-0.5, -0.5, 0};
    rect_vertices[3].position = {-0.5, 0.5, 0};

    rect_vertices[0].colour   = {0, 0, 0, 1};
    rect_vertices[1].colour   = {0.5, 0.5, 0.5, 1};
    rect_vertices[2].colour   = {1, 0, 0, 1};
    rect_vertices[3].colour   = {0, 1, 0, 1};

    std::array<std::uint32_t, 6> rect_indices{};

    rect_indices[0] = 0;
    rect_indices[1] = 1;
    rect_indices[2] = 2;

    rect_indices[3] = 2;
    rect_indices[4] = 1;
    rect_indices[5] = 3;

    rectangle_      = upload_mesh(rect_indices, rect_vertices).get();
    deletion_queue_.push(rectangle_);
}

auto ENGINE_NS::GraphicsEngine::draw_imgui_(VkCommandBuffer cmd, VkImageView image) -> void {
    auto lock                                   = imgui.read();

    VkRenderingAttachmentInfo colour_attachment = attachment_info(image, nullptr, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    VkRenderingInfo render_info                 = rendering_info(
        VkExtent2D{.width = static_cast<unsigned int>(window_extent_.x), .height = static_cast<unsigned int>(window_extent_.y)},
        &colour_attachment, nullptr);

    if (lock.get().latest_draw) {
        vkCmdBeginRendering(cmd, &render_info);
        ImGui_ImplVulkan_RenderDrawData(lock.get().latest_draw, cmd);
        vkCmdEndRendering(cmd);
    }
}

auto ENGINE_NS::GraphicsEngine::draw_background_(VkCommandBuffer cmd) -> void {
    VkClearColorValue clear_value;
    clear_value = {
      {255.f / 255.f, 105.f / 255.f, 180.f / 255.f, 1.f}
    };

    VkImageSubresourceRange clear_range = image_subresource_range(VK_IMAGE_ASPECT_COLOR_BIT);
    vkCmdClearColorImage(cmd, draw_image_.image, VK_IMAGE_LAYOUT_GENERAL, &clear_value, 1, &clear_range);

    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, gradient_pipeline_.pipeline);
    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, gradient_pipeline_.pipeline_layout, 0, 1, &draw_image_descriptors_, 0,
                            nullptr);
    vkCmdDispatch(cmd, (std::uint32_t)std::ceil(window_extent_.x / 16.0), (std::uint32_t)std::ceil(window_extent_.y / 16.0), 1);
}

auto ENGINE_NS::GraphicsEngine::draw_geometry_(VkCommandBuffer cmd) -> void {
    VkRenderingAttachmentInfo colour_attachment = attachment_info(draw_image_.view, nullptr, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    VkRenderingInfo render_info                 = rendering_info(
        VkExtent2D{.width = static_cast<unsigned int>(window_extent_.x), .height = static_cast<unsigned int>(window_extent_.y)},
        &colour_attachment, nullptr);

    vkCmdBeginRendering(cmd, &render_info);
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, triangle_pipeline_.pipeline);

    VkViewport viewport = {};
    viewport.x          = 0;
    viewport.y          = 0;
    viewport.width      = static_cast<float>(window_extent_.x);
    viewport.height     = static_cast<float>(window_extent_.y);
    viewport.minDepth   = 0.f;
    viewport.maxDepth   = 1.f;

    vkCmdSetViewport(cmd, 0, 1, &viewport);

    VkRect2D scissor      = {};
    scissor.offset.x      = 0;
    scissor.offset.y      = 0;
    scissor.extent.width  = window_extent_.x;
    scissor.extent.height = window_extent_.y;

    vkCmdSetScissor(cmd, 0, 1, &scissor);

    vkCmdDraw(cmd, 3, 1, 0, 0);

    if (rectangle_.index_buffer.buffer != VK_NULL_HANDLE) {
        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, mesh_pipeline_.pipeline);

        GPUDrawPushConstants push_constants{};
        push_constants.world_matrix  = ::linalg::Matrix4<float>::identity();
        push_constants.vertex_buffer = rectangle_.vertex_buffer_address;

        vkCmdPushConstants(cmd, mesh_pipeline_.layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(GPUDrawPushConstants), &push_constants);
        vkCmdBindIndexBuffer(cmd, rectangle_.index_buffer.buffer, 0, VK_INDEX_TYPE_UINT32);
        vkCmdDrawIndexed(cmd, 6, 1, 0, 0, 0);
    }

    vkCmdEndRendering(cmd);
}

auto ENGINE_NS::GraphicsEngine::draw_() -> void {
    tracy::SetThreadName(StaticNames::GraphicsThreadName);
    init_thread_(graphics::Thread::DRAW);
    constexpr std::uint32_t TIMEOUT = 250'000'000;

    while (!initialised_) {
    }

    while (running_.load(std::memory_order_acquire)) {
        FrameMarkStart(StaticNames::RenderLoop);
        auto frame_start = std::chrono::high_resolution_clock::now();
        {
            auto frame = current_frame().read();
            VK_CHECK(vkWaitForFences(device_.device, 1, &frame.get().render_fence_, true, TIMEOUT));
        }
        {
            auto frame = current_frame().write();
            VK_CHECK(vkResetFences(device_.device, 1, &frame.get().render_fence_));
            frame.get().deletion_queue.flush(device_, allocator_);

            std::uint32_t swapchain_image_index = 0;
            VK_CHECK(vkAcquireNextImageKHR(device_.device, swapchain_.swapchain, TIMEOUT, frame.get().swapchain_semaphore_, VK_NULL_HANDLE,
                                           &swapchain_image_index));

            VkCommandBuffer cmd = frame.get().main_command_buffer;
            VK_CHECK(vkResetCommandBuffer(cmd, 0));
            VkCommandBufferBeginInfo cmd_begin_info = command_buffer_begin_info(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

            VkExtent2D draw_extent                  = {draw_image_.extent.width, draw_image_.extent.height};

            VK_CHECK(vkBeginCommandBuffer(cmd, &cmd_begin_info));
            TracyVkCollect(frame.get().tracy_context_, cmd);
            {
                TracyVkZone(frame.get().tracy_context_, cmd, StaticNames::MainCommandBufferName);

                transition_image(cmd, draw_image_.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

                draw_background_(cmd);

                transition_image(cmd, draw_image_.image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

                draw_geometry_(cmd);

                transition_image(cmd, draw_image_.image, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
                transition_image(cmd, swapchain_.images[swapchain_image_index], VK_IMAGE_LAYOUT_UNDEFINED,
                                 VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);


                blit_image(cmd, draw_image_.image, swapchain_.images[swapchain_image_index], draw_extent, swapchain_.extent);
                draw_imgui_(cmd, swapchain_.views[swapchain_image_index]);

                transition_image(cmd, swapchain_.images[swapchain_image_index], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                 VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
            }
            VK_CHECK(vkEndCommandBuffer(cmd));

            VkCommandBufferSubmitInfo cmd_info = command_buffer_submit_info(cmd);

            VkSemaphoreSubmitInfo wait_info =
                semaphore_submit_info(VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, frame.get().swapchain_semaphore_);

            VkSemaphoreSubmitInfo signal_info =
                semaphore_submit_info(VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT, swapchain_.semaphores[swapchain_image_index]);

            VkSubmitInfo2 submit_info = ENGINE_NS::submit_info(&cmd_info, &signal_info, &wait_info);

            VK_CHECK(vkQueueSubmit2(graphics_queue_, 1, &submit_info, frame.get().render_fence_));

            VkPresentInfoKHR present_info   = {};
            present_info.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            present_info.pSwapchains        = &swapchain_.swapchain;
            present_info.swapchainCount     = 1;
            present_info.pWaitSemaphores    = &swapchain_.semaphores[swapchain_image_index];
            present_info.waitSemaphoreCount = 1;
            present_info.pImageIndices      = &swapchain_image_index;

            VK_CHECK(vkQueuePresentKHR(graphics_queue_, &present_info));
        }
        auto frame_delta = std::chrono::high_resolution_clock::now() - frame_start;
        if (frame_delta < this->update_rate_) {
            auto sleep = this->update_rate_ - frame_delta;
            std::this_thread::sleep_for(sleep);
        }
        ++frame_number_;
        FrameMarkEnd(StaticNames::RenderLoop);
    }
}

auto ENGINE_NS::GraphicsEngine::upload_() -> void {
    tracy::SetThreadName(StaticNames::UploadThreadName);
    init_thread_(graphics::Thread::UPLOAD);

    while (!initialised_) {
    }

    struct StagingBuffer {
            BufferAllocation allocation{};
            void* mapped_data      = nullptr;
            std::size_t total_size = 0;
    };
    std::vector<StagingBuffer> staging_buffers;
    constexpr std::size_t MAX_CACHED_STAGING_BUFFER_SIZE = 1ull * 1'024 * 1'024 * 1'024;

    while (running_.load(std::memory_order_acquire)) {
        FrameMarkStart(StaticNames::UploadLoop);
        auto enter_time       = std::chrono::steady_clock::now();
        bool has_time_elapsed = false;
        while (running_.load(std::memory_order_acquire) && !upload_ready_.load(std::memory_order_acquire)) {
            auto dt = std::chrono::steady_clock::now() - enter_time;
            if (!has_time_elapsed && dt > std::chrono::minutes(1)) {
                {
                    auto logger = g_ENGINE->logger.get(LogNamespaces::GRAPHICS);
                    logger.get().debug("Freeing cached staging buffers");
                }
                has_time_elapsed = true;
                for (auto& staging : staging_buffers) {
                    upload_deletion_queue_.push(staging.allocation);
                    vmaUnmapMemory(allocator_, staging.allocation.allocation);
                }
                upload_deletion_queue_.flush(device_, allocator_);
                staging_buffers.clear();
            }
            if (dt > std::chrono::seconds(1)) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            } else {
                std::this_thread::yield();
            }
        }
        if (!running_) {
            break;
        }


        auto lock     = uploads_.write();
        auto& uploads = lock.get();
        {
            auto logger = g_ENGINE->logger.get(LogNamespaces::GRAPHICS);
            logger.get().debug("Uploading {} meshes", uploads.size());
        }
        while (!uploads.empty()) {
            ZoneScoped;
            auto& mesh                           = uploads.back();

            const std::size_t vertex_buffer_size = mesh.vertices.size() * sizeof(Vertex);
            const std::size_t index_buffer_size  = mesh.indices.size() * sizeof(std::uint32_t);

            GPUMeshBuffers new_surface{};
            new_surface.vertex_buffer = allocate_buffer(
                vertex_buffer_size,
                VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
                VMA_MEMORY_USAGE_GPU_ONLY);

            VkBufferDeviceAddressInfo device_address_info{};
            device_address_info.sType         = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
            device_address_info.buffer        = new_surface.vertex_buffer.buffer;
            new_surface.vertex_buffer_address = vkGetBufferDeviceAddress(device_.device, &device_address_info);

            new_surface.index_buffer          = allocate_buffer(
                index_buffer_size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY);

            const auto desired_size = vertex_buffer_size + index_buffer_size;
            StagingBuffer* staging  = nullptr;
            for (auto& buffer : staging_buffers) {
                if (buffer.total_size >= desired_size) {
                    staging = &buffer;
                    break;
                }
            }
            if (!staging) {
                auto alloc_size = desired_size + (1'024 - (desired_size % 1'024));
                staging_buffers.push_back(
                    {allocate_buffer(alloc_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY), nullptr, alloc_size});
                vmaMapMemory(allocator_, staging_buffers.back().allocation.allocation, &staging_buffers.back().mapped_data);
                staging = &staging_buffers.back();
            }

            std::memcpy(staging->mapped_data, mesh.vertices.data(), vertex_buffer_size);
            std::memcpy(static_cast<std::uint8_t*>(staging->mapped_data) + vertex_buffer_size, mesh.indices.data(), index_buffer_size);

            immediate_submit([&](VkCommandBuffer cmd) {
                VkBufferCopy vertex_copy{0};
                vertex_copy.dstOffset = 0;
                vertex_copy.srcOffset = 0;
                vertex_copy.size      = vertex_buffer_size;

                vkCmdCopyBuffer(cmd, staging->allocation.buffer, new_surface.vertex_buffer.buffer, 1, &vertex_copy);

                VkBufferCopy index_copy{0};
                index_copy.dstOffset = 0;
                index_copy.srcOffset = vertex_buffer_size;
                index_copy.size      = index_buffer_size;

                vkCmdCopyBuffer(cmd, staging->allocation.buffer, new_surface.index_buffer.buffer, 1, &index_copy);
            });

            mesh.promise.set_value(std::move(new_surface));

            uploads.pop_back();
        }
        lock.drop();

        {
            ZoneScoped;
            std::sort(staging_buffers.begin(), staging_buffers.end(), [](const StagingBuffer& lhs, const StagingBuffer& rhs) {
                return lhs.total_size > rhs.total_size;
            });

            std::size_t current_size = 0;
            for (auto& buffer : staging_buffers) {
                current_size += buffer.total_size;
            }
            while (current_size > MAX_CACHED_STAGING_BUFFER_SIZE) {
                current_size -= staging_buffers.back().total_size;
                upload_deletion_queue_.push(staging_buffers.back().allocation);
                vmaUnmapMemory(allocator_, staging_buffers.back().allocation.allocation);
                staging_buffers.pop_back();
            }
            upload_deletion_queue_.flush(device_, allocator_);
        }
        upload_ready_ = false;
        FrameMarkEnd(StaticNames::UploadLoop);
    }

    for (auto& buffer : staging_buffers) {
        upload_deletion_queue_.push(buffer.allocation);
        vmaUnmapMemory(allocator_, buffer.allocation.allocation);
    }
}

auto ENGINE_NS::graphics::thread_name(Thread thread) -> std::string {
    switch (thread) {
        case Thread::MAIN:
            return "main";
        case Thread::DRAW:
            return "draw";
        case Thread::UPLOAD:
            return "upload";
    }
    std::unreachable();
}

auto ENGINE_NS::graphics::thread_immediate_name(Thread thread) -> std::string {
    return fmt::format("{}_{}", IMMEDIATE_NAME, thread_name(thread));
}
