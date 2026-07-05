#include "engine/graphics/graphics.h"

#include "engine/assets/library.h"
#include "engine/deletion_queue.h"
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
#include <fmt/format.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_vulkan.h>
#include <robin_set.h>
#include <vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>

#include <tracy/Tracy.hpp>
#include <tracy/TracyVulkan.hpp>
#include <common/TracySystem.hpp>
#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <span>
#include <utility>
#include <vector>

void ENGINE_NS::GraphicsEngine::initialise() {
    FrameMarkStart(StaticNames::GraphicsInit);
    auto logger = ENGINE_NS::g_ENGINE->logger.get(ENGINE_NS::LogNamespaces::GRAPHICS);
    logger.get().info("Creating graphics engine");

    logger.get().info("Initialising SDL");
    SDL_Init(SDL_INIT_VIDEO);
    {
        ZoneScoped;
        SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN);

        window_ = SDL_CreateWindow("Vulkan Engine", window_extent_.x, window_extent_.y, window_flags);
    }

    logger.get().info("Initialising Vulkan");
    init_vulkan_();

    logger.get().info("Initialising render thread");
    {
        ZoneScoped;
        running_.store(true, std::memory_order_release);
        update_rate_   = std::chrono::milliseconds(8);
        render_thread_ = std::thread(&ENGINE_NS::GraphicsEngine::draw_, this);
    }

    logger.get().info("Initialising upload thread");
    {
        ZoneScoped;
        upload_ready_.store(false, std::memory_order_release);
        upload_thread_ = std::thread(&ENGINE_NS::GraphicsEngine::upload_, this);
    }

    logger.get().info("Initialising pipeline compile thread");
    {
        ZoneScoped;
        pipeline_compile_thread_ = std::thread(&ENGINE_NS::GraphicsEngine::compile_, this);
    }

    init_immediates_();

    initialised_ = true;

    FrameMarkEnd(StaticNames::GraphicsInit);
}

void ENGINE_NS::GraphicsEngine::draw() {
    FrameMarkStart(StaticNames::GraphicsDraw);
    frame_deletion_queue_.flush(device_, allocator_);

    {
        auto lock = imgui.write();
        ImGui::Render();
        auto* draw_data = ImGui::GetDrawData();
        if (draw_data != nullptr) {
            lock.get().latest_draw = draw_data;
        }
    }

    FrameMarkEnd(StaticNames::GraphicsDraw);
}

void ENGINE_NS::GraphicsEngine::cleanup() {
    FrameMarkStart(StaticNames::GraphicsDeinit);
    auto logger = ENGINE_NS::g_ENGINE->logger.get(ENGINE_NS::LogNamespaces::GRAPHICS);
    logger.get().info("Cleaning up graphics engine");
    if (g_ENGINE->crashed) {
        logger.get().warning("Opening all locked resources by force");
        imgui.unsafe_open_all_locks();
        registered_pipelines_.unsafe_open_all_locks();
        in_use_pipelines_.unsafe_open_all_locks();
        to_delete_pipelines_.unsafe_open_all_locks();
        mesh_uploads_.unsafe_open_all_locks();
        for (auto& frame : frames_) {
            frame.unsafe_open_all_locks();
        }
    }

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
    logger.get().info("Stopping compile thread");
    {
        ZoneScoped;
        this->pipeline_compile_condition_.notify_all();
        this->pipeline_compile_thread_.join();
    }

    logger.get().info("Cleaning up Vulkan");
    if (device_.device != VK_NULL_HANDLE) {
        vkDeviceWaitIdle(device_.device);
        {
            ZoneScopedN(StaticNames::DeleteRegisteredPipelines);
            auto to_delete_pipelines = to_delete_pipelines_.read();
            for (const auto& to_delete : to_delete_pipelines.get()) {
                to_delete->destroy(device_, allocator_);
            }

            auto registered_pipelines = registered_pipelines_.read();
            for (const auto& [_, pipeline] : registered_pipelines.get()) {
                pipeline->destroy(device_, allocator_);
            }
        }

        for (std::size_t idx = 0; idx < graphics::FRAME_OVERLAP; idx++) {
            auto frame = frames_[idx].write();
            vkDestroySemaphore(device_.device, frame.get().swapchain_semaphore_, nullptr);
            vkDestroyFence(device_.device, frame.get().render_fence_, nullptr);

            if (frame.get().tracy_context_ != nullptr) {
                TracyVkDestroy(frame.get().tracy_context_);
            }
            vkDestroyCommandPool(device_.device, frame.get().command_pool, nullptr);

            frame.get().deletion_queue.flush(device_, allocator_);
            frame.get().descriptor_allocator.clear_pools(device_);
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
    return frames_[frame_number_.load(std::memory_order_acquire) % graphics::FRAME_OVERLAP];
}

auto ENGINE_NS::GraphicsEngine::next_frame() -> RwLock<graphics::FrameData>& {
    return frames_[(frame_number_.load(std::memory_order_acquire) + 1) % graphics::FRAME_OVERLAP];
}

auto ENGINE_NS::GraphicsEngine::allocate_buffer(std::size_t size, VkBufferUsageFlags flags, VmaMemoryUsage usage) -> BufferAllocation {
    VkBufferCreateInfo buffer_info{};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size  = size;
    buffer_info.usage = flags;

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

auto ENGINE_NS::GraphicsEngine::allocate_image(VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped)
    -> ImageAllocation {
    ImageAllocation new_image{};
    new_image.format = format;
    new_image.extent = size;

    VkImageCreateInfo image_info = image_create_info(format, usage, size);
    if (mipmapped) {
        image_info.mipLevels = static_cast<std::uint32_t>(std::floor(std::log2(std::max(size.width, size.height)))) + 1;
    }

    VmaAllocationCreateInfo allocation_info{};
    allocation_info.usage         = VMA_MEMORY_USAGE_GPU_ONLY;
    allocation_info.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VK_CHECK(vmaCreateImage(allocator_, &image_info, &allocation_info, &new_image.image, &new_image.allocation, nullptr));

    VkImageAspectFlags aspect_flag = VK_IMAGE_ASPECT_COLOR_BIT;
    if (format == VK_FORMAT_D32_SFLOAT) {
        aspect_flag = VK_IMAGE_ASPECT_DEPTH_BIT;
    }

    VkImageViewCreateInfo view_info       = image_view_create_info(format, new_image.image, aspect_flag);
    view_info.subresourceRange.levelCount = image_info.mipLevels;

    VK_CHECK(vkCreateImageView(device_.device, &view_info, nullptr, &new_image.view));

    return new_image;
}

auto ENGINE_NS::GraphicsEngine::destroy_image(ImageAllocation allocation) -> void {
    frame_deletion_queue_.push(allocation);
}

auto ENGINE_NS::GraphicsEngine::destroy_shader(engine::asset::CompiledShader shader) -> void {
    frame_deletion_queue_.push(shader);
}

auto ENGINE_NS::GraphicsEngine::upload_mesh(std::span<std::uint32_t> indices, std::span<Vertex> vertices) -> std::future<GPUMeshBuffers> {
    graphics::MeshUpload upload{};
    upload.indices  = std::vector<std::uint32_t>(indices.begin(), indices.end());
    upload.vertices = std::vector<Vertex>(vertices.begin(), vertices.end());

    auto lock     = mesh_uploads_.write();
    auto& uploads = lock.get();
    uploads.push_back(std::move(upload));
    auto future = uploads.back().promise.get_future();

    upload_ready_.store(true, std::memory_order_release);
    return future;
}

auto ENGINE_NS::GraphicsEngine::upload_image(void* data, VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped)
    -> std::future<ImageAllocation> {
    graphics::TextureUpload upload{};
    upload.format       = format;
    upload.mipmapped    = mipmapped;
    upload.size         = size;
    upload.texture_data = data;
    upload.usage        = usage;

    auto lock     = texture_uploads_.write();
    auto& uploads = lock.get();
    uploads.push_back(std::move(upload));
    auto future = uploads.back().promise.get_future();

    upload_ready_.store(true, std::memory_order_release);

    return future;
}

auto ENGINE_NS::GraphicsEngine::pause_registered_pipelines() -> void {
    auto registered_pipelines = registered_pipelines_.write();
    for (const auto& [_, pipeline] : registered_pipelines.get()) {
        pipeline->paused_.fetch_add(1, std::memory_order_release);
    }
}

auto ENGINE_NS::GraphicsEngine::resume_registered_pipelines() -> void {
    auto registered_pipelines = registered_pipelines_.write();
    for (const auto& [_, pipeline] : registered_pipelines.get()) {
        pipeline->paused_.fetch_sub(1, std::memory_order_release);
    }
}

auto ENGINE_NS::GraphicsEngine::register_pipelines(std::vector<std::unique_ptr<graphics::RegisteredPipeline>>&& pipelines)
    -> graphics::RegisteredPipelineReceipt {
    std::vector<std::uint64_t> ids;
    auto new_pipelines = new_pipelines_.write();
    for (auto& pipeline : pipelines) {
        std::uint64_t pipeline_uid = this->next_pipeline_uid_++;
        pipeline->id_              = pipeline_uid;
        new_pipelines.get().push_back(std::move(pipeline));
    }
    pipeline_compile_condition_.notify_one();
    return graphics::RegisteredPipelineReceipt(*this, std::move(ids));
}

auto ENGINE_NS::GraphicsEngine::deregister_pipelines(std::vector<std::uint64_t>& ids) -> void {
    {
        auto ids_set = tsl::robin_set<std::uint64_t>{ids.begin(), ids.end()};

        auto new_pipelines_lock = new_pipelines_.write();
        auto& new_pipelines     = new_pipelines_lock.get();
        new_pipelines.erase(
            std::remove_if(new_pipelines.begin(),
                           new_pipelines.end(),
                           [&ids_set](std::unique_ptr<graphics::RegisteredPipeline>& pipeline) { return ids_set.contains(pipeline->id_); }),
            new_pipelines.end());
    }

    {
        auto to_delete_pipelines  = to_delete_pipelines_.write();
        auto registered_pipelines = registered_pipelines_.write();
        for (auto& pipeline : ids) {
            to_delete_pipelines.get().emplace_back(std::move(registered_pipelines.get().at(pipeline)));
            registered_pipelines.get().erase(pipeline);
        }
    }
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
    VkPhysicalDeviceVulkan13Features features{};
    features.sType            = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    features.dynamicRendering = VK_TRUE;
    features.synchronization2 = VK_TRUE;

    // vulkan 1.2 features
    VkPhysicalDeviceVulkan12Features features12{};
    features12.sType               = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    features12.bufferDeviceAddress = VK_TRUE;
    features12.descriptorIndexing  = VK_TRUE;

    VkPhysicalDeviceVulkan11Features features11{};
    features11.sType                = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
    features11.shaderDrawParameters = VK_TRUE;

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
            .request_queue(graphics::thread_immediate_name(graphics::Thread::COMPILE), VulkanQueueType::TRANSFER | VulkanQueueType::COMPUTE)
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

    graphics_queue_ = device_.queues.at("graphics").get();
    transfer_queue_ = device_.queues.at("transfer").get();

    imgui.write().get().queue = device_.queues.at("imgui").get();

    VkCommandPoolCreateInfo pool_info =
        command_pool_create_info(device_.queues.at("graphics").family, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    VkSemaphoreCreateInfo semaphore_info = semaphore_create_info(0);
    VkFenceCreateInfo fence_info         = fence_create_info(VK_FENCE_CREATE_SIGNALED_BIT);

    logger.get().debug("Initialising sync structures");
    for (std::size_t idx = 0; idx < graphics::FRAME_OVERLAP; idx++) {
        auto frame = frames_[idx].write();
        VK_CHECK(vkCreateCommandPool(device_.device, &pool_info, nullptr, &frame.get().command_pool));

        VkCommandBufferAllocateInfo buffer_alloc = command_buffer_allocate_info(frame.get().command_pool);
        VK_CHECK(vkAllocateCommandBuffers(device_.device, &buffer_alloc, &frame.get().main_command_buffer));
        frame.get().tracy_context_ =
            TracyVkContext(vulkan_instance_.instance, physical_device_.device, device_.device, device_.queues.at("graphics").get(), frame.get().main_command_buffer, vkGetInstanceProcAddr, vkGetDeviceProcAddr);

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

    VkExtent3D draw_extent = {.width  = static_cast<std::uint32_t>(window_extent_.x),
                              .height = static_cast<std::uint32_t>(window_extent_.y),
                              .depth  = 1};
    draw_image_.format     = VK_FORMAT_R16G16B16A16_SFLOAT;
    draw_image_.extent     = draw_extent;

    VkImageUsageFlags draw_image_usages{};
    draw_image_usages |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    draw_image_usages |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    draw_image_usages |= VK_IMAGE_USAGE_STORAGE_BIT;
    draw_image_usages |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    VkImageCreateInfo draw_img_info = image_create_info(draw_image_.format, draw_image_usages, draw_extent);

    VmaAllocationCreateInfo draw_img_allocation = {};
    draw_img_allocation.usage                   = VMA_MEMORY_USAGE_GPU_ONLY;
    draw_img_allocation.requiredFlags           = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    vmaCreateImage(allocator_, &draw_img_info, &draw_img_allocation, &draw_image_.image, &draw_image_.allocation, nullptr);

    VkImageViewCreateInfo draw_image_view = image_view_create_info(draw_image_.format, draw_image_.image, VK_IMAGE_ASPECT_COLOR_BIT);

    VK_CHECK(vkCreateImageView(device_.device, &draw_image_view, nullptr, &draw_image_.view));

    deletion_queue_.push(draw_image_);
}

auto ENGINE_NS::GraphicsEngine::init_descriptors_() -> void {
    std::vector<DescriptorAllocatorGrowable::PoolSizeRatio> sizes = {
      {.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,          .ratio = 3},
      {.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,         .ratio = 3},
      {.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         .ratio = 3},
      {.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, .ratio = 4},
    };
    global_descriptor_allocator_.init(device_, 100, sizes);
    deletion_queue_.push(global_descriptor_allocator_);

    for (std::size_t idx = 0; idx < graphics::FRAME_OVERLAP; idx++) {
        auto frame_lock = frames_[idx].write();
        frame_lock.get().descriptor_allocator.init(device_, 1'000, sizes);
        deletion_queue_.push(frame_lock.get().descriptor_allocator);
    }
}

auto ENGINE_NS::GraphicsEngine::init_imgui_() -> void {
    IMGUI_CHECKVERSION();
    auto& imgui_                                 = imgui.write().get();
    std::vector<VkDescriptorPoolSize> pool_sizes = {
      {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4 * IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE},
    };

    VkDescriptorPoolCreateInfo pool_info{};
    pool_info.sType   = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags   = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 0;
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
    ImGui_ImplVulkan_InitInfo init_info{};
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
    for (const auto& thread : {graphics::Thread::MAIN, graphics::Thread::DRAW, graphics::Thread::UPLOAD}) {
        auto& immediate = immediates_.insert({thread, {}}).first.value();

        VkFenceCreateInfo fence_info = fence_create_info(VK_FENCE_CREATE_SIGNALED_BIT);
        VK_CHECK(vkCreateFence(device_.device, &fence_info, nullptr, &immediate.fence));

        const auto& queue = device_.queues.at(graphics::thread_immediate_name(thread));
        immediate.queue   = queue.get();

        VkCommandPoolCreateInfo pool_info = command_pool_create_info(queue.family, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

        vkCreateCommandPool(device_.device, &pool_info, nullptr, &immediate.command_pool);
        VkCommandBufferAllocateInfo command_alloc_info = command_buffer_allocate_info(immediate.command_pool);

        VK_CHECK(vkAllocateCommandBuffers(device_.device, &command_alloc_info, &immediate.command_buffer));

        immediate.tracy_context = TracyVkContext(vulkan_instance_.instance, physical_device_.device, device_.device, queue.get(), immediate.command_buffer, vkGetInstanceProcAddr, vkGetDeviceProcAddr);

        deletion_queue_.push(immediate);
    }
}

auto ENGINE_NS::GraphicsEngine::init_pipelines_() -> void {
}

auto ENGINE_NS::GraphicsEngine::draw_imgui_(VkCommandBuffer cmd, VkImageView image) -> void {
    auto lock = imgui.read();

    VkRenderingAttachmentInfo colour_attachment = attachment_info(image, nullptr, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    VkRenderingInfo render_info                 = rendering_info(
        VkExtent2D{.width = static_cast<unsigned int>(window_extent_.x), .height = static_cast<unsigned int>(window_extent_.y)},
        &colour_attachment,
        nullptr);

    if (lock.get().latest_draw != nullptr) {
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
}

auto ENGINE_NS::GraphicsEngine::draw_registered_(RwDataMut<graphics::FrameData>& frame, VkCommandBuffer cmd) -> void {
    ZoneScoped;
    VkRenderingAttachmentInfo colour_attachment = attachment_info(draw_image_.view, nullptr, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    VkRenderingInfo render_info                 = rendering_info(
        VkExtent2D{.width = static_cast<unsigned int>(window_extent_.x), .height = static_cast<unsigned int>(window_extent_.y)},
        &colour_attachment,
        nullptr);

    vkCmdBeginRendering(cmd, &render_info);

    VkViewport viewport{};
    viewport.x        = 0;
    viewport.y        = 0;
    viewport.width    = static_cast<float>(window_extent_.x);
    viewport.height   = static_cast<float>(window_extent_.y);
    viewport.minDepth = 0.f;
    viewport.maxDepth = 1.f;

    vkCmdSetViewport(cmd, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset.x      = 0;
    scissor.offset.y      = 0;
    scissor.extent.width  = window_extent_.x;
    scissor.extent.height = window_extent_.y;

    vkCmdSetScissor(cmd, 0, 1, &scissor);

    auto registered_pipelines = registered_pipelines_.read();
    for (const auto& [id, pipeline] : registered_pipelines.get()) {
        ZoneScoped;
        if (!pipeline->enabled) {
            continue;
        }
        auto paused_count = pipeline->paused_.load(std::memory_order_acquire);
        if (paused_count > 0) {
            continue;
        }

        if (pipeline->graphics_pipeline_.has_value()) {
            vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->graphics_pipeline_.value().pipeline);

            auto push_constants = pipeline->push_constants();
            if (push_constants.size > 0 && (push_constants.data != nullptr)) {
                vkCmdPushConstants(cmd,
                                   pipeline->graphics_pipeline_.value().layout,
                                   VK_SHADER_STAGE_VERTEX_BIT,
                                   0,
                                   static_cast<std::uint32_t>(push_constants.size),
                                   push_constants.data);
            }
            pipeline->record_graphics(cmd);
        }

        if (pipeline->compute_pipeline_.has_value()) {
            vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline->compute_pipeline_.value().pipeline);

            auto push_constants = pipeline->push_constants();
            if (push_constants.size > 0 && (push_constants.data != nullptr)) {
                vkCmdPushConstants(cmd,
                                   pipeline->compute_pipeline_.value().layout,
                                   VK_SHADER_STAGE_VERTEX_BIT,
                                   0,
                                   static_cast<std::uint32_t>(push_constants.size),
                                   push_constants.data);
            }
            pipeline->record_compute(cmd);
        }

        frame.get().in_use_pipelines.push_back(id);
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
            ZoneScoped;
            auto frame = current_frame().write();
            VK_CHECK(vkResetFences(device_.device, 1, &frame.get().render_fence_));
            frame.get().deletion_queue.flush(device_, allocator_);
            frame.get().descriptor_allocator.clear_pools(device_);

            std::uint32_t swapchain_image_index = 0;
            VK_CHECK(vkAcquireNextImageKHR(device_.device,
                                           swapchain_.swapchain,
                                           TIMEOUT,
                                           frame.get().swapchain_semaphore_,
                                           VK_NULL_HANDLE,
                                           &swapchain_image_index));

            VkCommandBuffer cmd = frame.get().main_command_buffer;
            VK_CHECK(vkResetCommandBuffer(cmd, 0));
            VkCommandBufferBeginInfo cmd_begin_info = command_buffer_begin_info(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

            VkExtent2D draw_extent = {.width = draw_image_.extent.width, .height = draw_image_.extent.height};

            VK_CHECK(vkBeginCommandBuffer(cmd, &cmd_begin_info));
            TracyVkCollect(frame.get().tracy_context_, cmd);
            {
                ZoneScoped;
                TracyVkZone(frame.get().tracy_context_, cmd, StaticNames::MainCommandBufferName);

                transition_image(cmd, draw_image_.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

                draw_background_(cmd);

                transition_image(cmd, draw_image_.image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

                {
                    auto in_use_pipelines = in_use_pipelines_.write();
                    for (auto& pipeline_id : frame.get().in_use_pipelines) {
                        in_use_pipelines.get().at(pipeline_id) -= 1;
                    }
                    frame.get().in_use_pipelines.clear();
                    draw_registered_(frame, cmd);
                    for (auto& pipeline_id : frame.get().in_use_pipelines) {
                        in_use_pipelines.get().at(pipeline_id) += 1;
                    }
                }

                transition_image(cmd, draw_image_.image, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
                transition_image(cmd,
                                 swapchain_.images[swapchain_image_index],
                                 VK_IMAGE_LAYOUT_UNDEFINED,
                                 VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);


                blit_image(cmd, draw_image_.image, swapchain_.images[swapchain_image_index], draw_extent, swapchain_.extent);
                draw_imgui_(cmd, swapchain_.views[swapchain_image_index]);

                transition_image(cmd,
                                 swapchain_.images[swapchain_image_index],
                                 VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
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

            VkPresentInfoKHR present_info{};
            present_info.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            present_info.pSwapchains        = &swapchain_.swapchain;
            present_info.swapchainCount     = 1;
            present_info.pWaitSemaphores    = &swapchain_.semaphores[swapchain_image_index];
            present_info.waitSemaphoreCount = 1;
            present_info.pImageIndices      = &swapchain_image_index;

            VK_CHECK(vkQueuePresentKHR(graphics_queue_, &present_info));
        }
        {
            ZoneScopedN(StaticNames::DeleteRegisteredPipelines);
            auto to_delete_pipelines = to_delete_pipelines_.read();
            auto in_use_pipelines    = in_use_pipelines_.write();
            for (const auto& to_delete : to_delete_pipelines.get()) {
                if (in_use_pipelines.get().at(to_delete->id_) != 0) {
                    continue;
                }
                in_use_pipelines.get().erase(to_delete->id_);
                to_delete->destroy(device_, allocator_);
            }
        }
        auto frame_delta = std::chrono::high_resolution_clock::now() - frame_start;
        if (frame_delta < this->update_rate_) {
            auto sleep = this->update_rate_ - frame_delta;
            std::this_thread::sleep_for(sleep);
        }
        frame_number_.fetch_add(1, std::memory_order_release);
        FrameMarkEnd(StaticNames::RenderLoop);
    }
}

auto ENGINE_NS::GraphicsEngine::upload_() -> void {
    tracy::SetThreadName(StaticNames::UploadThreadName);
    init_thread_(graphics::Thread::UPLOAD);

    while (!initialised_) {
    }

    std::vector<graphics::StagingBuffer> staging_buffers;
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

        upload_meshes_(staging_buffers);
        upload_textures_(staging_buffers);

        {
            ZoneScoped;
            std::sort(
                staging_buffers.begin(),
                staging_buffers.end(),
                [](const graphics::StagingBuffer& lhs, const graphics::StagingBuffer& rhs) { return lhs.total_size > rhs.total_size; });

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

auto ENGINE_NS::GraphicsEngine::upload_meshes_(std::vector<graphics::StagingBuffer>& staging_buffers) -> void {
    ZoneScoped;
    auto lock     = mesh_uploads_.write();
    auto& uploads = lock.get();
    {
        auto logger = g_ENGINE->logger.get(LogNamespaces::GRAPHICS);
        logger.get().debug("Uploading {} meshes", uploads.size());
    }
    while (!uploads.empty()) {
        ZoneScoped;
        auto& mesh = uploads.back();

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

        new_surface.index_buffer = allocate_buffer(index_buffer_size,
                                                   VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                                   VMA_MEMORY_USAGE_GPU_ONLY);

        const auto desired_size          = vertex_buffer_size + index_buffer_size;
        graphics::StagingBuffer* staging = nullptr;
        for (auto& buffer : staging_buffers) {
            if (buffer.total_size >= desired_size) {
                staging = &buffer;
                break;
            }
        }
        if (staging == nullptr) {
            auto alloc_size = desired_size + (1'024 - (desired_size % 1'024));
            staging_buffers.push_back(
                {.allocation  = allocate_buffer(alloc_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY),
                 .mapped_data = nullptr,
                 .total_size  = alloc_size});
            vmaMapMemory(allocator_, staging_buffers.back().allocation.allocation, &staging_buffers.back().mapped_data);
            staging = &staging_buffers.back();
        }

        std::memcpy(staging->mapped_data, mesh.vertices.data(), vertex_buffer_size);
        std::memcpy(static_cast<std::uint8_t*>(staging->mapped_data) + vertex_buffer_size, mesh.indices.data(), index_buffer_size);

        immediate_submit([&](VkCommandBuffer cmd) {
            VkBufferCopy vertex_copy{};
            vertex_copy.dstOffset = 0;
            vertex_copy.srcOffset = 0;
            vertex_copy.size      = vertex_buffer_size;

            vkCmdCopyBuffer(cmd, staging->allocation.buffer, new_surface.vertex_buffer.buffer, 1, &vertex_copy);

            VkBufferCopy index_copy{};
            index_copy.dstOffset = 0;
            index_copy.srcOffset = vertex_buffer_size;
            index_copy.size      = index_buffer_size;

            vkCmdCopyBuffer(cmd, staging->allocation.buffer, new_surface.index_buffer.buffer, 1, &index_copy);
        });

        mesh.promise.set_value(std::move(new_surface));

        uploads.pop_back();
    }
}

auto ENGINE_NS::GraphicsEngine::upload_textures_(std::vector<graphics::StagingBuffer>& staging_buffers) -> void {
    ZoneScoped;
    auto lock     = texture_uploads_.write();
    auto& uploads = lock.get();
    {
        auto logger = g_ENGINE->logger.get(LogNamespaces::GRAPHICS);
        logger.get().debug("Uploading {} textures", uploads.size());
    }
    while (!uploads.empty()) {
        ZoneScoped;
        auto& texture = uploads.back();

        std::size_t data_size            = static_cast<std::size_t>(texture.size.width * texture.size.depth * texture.size.height * 4);
        graphics::StagingBuffer* staging = nullptr;
        for (auto& buffer : staging_buffers) {
            if (buffer.total_size >= data_size) {
                staging = &buffer;
                break;
            }
        }
        if (staging == nullptr) {
            auto alloc_size = data_size + (1'024 - (data_size % 1'024));
            staging_buffers.push_back(
                {.allocation  = allocate_buffer(alloc_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY),
                 .mapped_data = nullptr,
                 .total_size  = alloc_size});
            vmaMapMemory(allocator_, staging_buffers.back().allocation.allocation, &staging_buffers.back().mapped_data);
            staging = &staging_buffers.back();
        }

        std::memcpy(staging->mapped_data, texture.texture_data, data_size);

        ImageAllocation new_image = allocate_image(texture.size,
                                                   texture.format,
                                                   texture.usage | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
                                                   texture.mipmapped);

        immediate_submit([&](VkCommandBuffer cmd) {
            transition_image(cmd, new_image.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

            VkBufferImageCopy copy_region{};
            copy_region.bufferOffset      = 0;
            copy_region.bufferRowLength   = 0;
            copy_region.bufferImageHeight = 0;

            copy_region.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
            copy_region.imageSubresource.mipLevel       = 0;
            copy_region.imageSubresource.baseArrayLayer = 0;
            copy_region.imageSubresource.layerCount     = 0;
            copy_region.imageExtent                     = texture.size;

            vkCmdCopyBufferToImage(cmd, staging->allocation.buffer, new_image.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy_region);

            transition_image(cmd, new_image.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        });

        texture.promise.set_value(std::move(new_image));
        uploads.pop_back();
    }
}

auto ENGINE_NS::GraphicsEngine::compile_() -> void {
    tracy::SetThreadName(StaticNames::CompileThreadName);
    init_thread_(graphics::Thread::COMPILE);

    while (!initialised_) {
    }

    auto logger = ENGINE_NS::g_ENGINE->logger.get(ENGINE_NS::LogNamespaces::GRAPHICS);
    while (running_.load(std::memory_order_acquire)) {
        std::unique_lock lock(pipeline_compile_lock_);
        pipeline_compile_condition_.wait(lock,
                                         [&] { return !running_.load(std::memory_order_acquire) || !new_pipelines_.read().get().empty(); });
        if (!running_.load(std::memory_order_acquire)) {
            break;
        }
        ZoneScopedN(StaticNames::CompileRun);

        {
            auto new_pipelines        = new_pipelines_.write();
            auto in_use_pipelines     = in_use_pipelines_.write();
            auto registered_pipelines = registered_pipelines_.write();
            logger.get().debug("Compiling {} pipeline(s)", new_pipelines.get().size());
            while (!new_pipelines.get().empty()) {
                ZoneScoped;
                std::unique_ptr<graphics::RegisteredPipeline> pipeline = std::move(new_pipelines.get().back());
                new_pipelines.get().pop_back();

                logger.get().debug(R"(Compiling pipeline "{}" with id "{}")", pipeline->name(), pipeline->id_);

                auto pipeline_id = pipeline->id_;
                pipeline->init_pipeline(*this, device_, allocator_);
                registered_pipelines.get()[pipeline_id] = std::move(pipeline);
                in_use_pipelines.get().insert({pipeline_id, 0});
            }
        }

        lock.unlock();
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
        case Thread::COMPILE:
            return "compile";
    }
    std::unreachable();
}

auto ENGINE_NS::graphics::thread_immediate_name(Thread thread) -> std::string {
    return fmt::format("{}_{}", IMMEDIATE_NAME, thread_name(thread));
}

ENGINE_NS::graphics::RegisteredPipelineReceipt::RegisteredPipelineReceipt(GraphicsEngine& engine, std::vector<std::uint64_t>&& ids) :
    engine_(engine), pipeline_ids_(std::move(ids)) {
}

ENGINE_NS::graphics::RegisteredPipelineReceipt::RegisteredPipelineReceipt(RegisteredPipelineReceipt&& other) noexcept :
    engine_(other.engine_), pipeline_ids_(std::move(other.pipeline_ids_)) {
    other.moved_ = true;
}

ENGINE_NS::graphics::RegisteredPipelineReceipt::~RegisteredPipelineReceipt() {
    if (moved_) {
        return;
    }
    engine_.deregister_pipelines(pipeline_ids_);
}

auto ENGINE_NS::graphics::RegisteredPipelineReceipt::operator=(RegisteredPipelineReceipt&& rhs) noexcept -> RegisteredPipelineReceipt& {
    if (this != &rhs) {
        this->pipeline_ids_ = std::move(rhs.pipeline_ids_);
    }
    rhs.moved_ = true;
    return *this;
}

ENGINE_NS::graphics::RegisteredPipeline::RegisteredPipeline(RegisteredPipeline&& other) noexcept :
    deletion_queue_(std::move(other.deletion_queue_)), graphics_pipeline_(std::move(other.graphics_pipeline_)),
    compute_pipeline_(std::move(other.compute_pipeline_)), id_(other.id_) {
    other.moved_ = true;
}

ENGINE_NS::graphics::RegisteredPipeline::~RegisteredPipeline() {
}

auto ENGINE_NS::graphics::RegisteredPipeline::operator=(RegisteredPipeline&& rhs) noexcept -> RegisteredPipeline& {
    if (this != &rhs) {
        this->deletion_queue_    = std::move(rhs.deletion_queue_);
        this->graphics_pipeline_ = std::move(rhs.graphics_pipeline_);
        this->compute_pipeline_  = std::move(rhs.compute_pipeline_);
        this->id_                = rhs.id_;
    }
    rhs.moved_ = true;
    return *this;
}

auto ENGINE_NS::graphics::RegisteredPipeline::build_graphics_pipeline(ENGINE_NS::GraphicsEngine& /* unused */,
                                                                      VulkanDevice& /* unused */,
                                                                      GraphicsRegisteredPipelineDeletionQueue& /*unused*/)
    -> std::optional<GraphicsPipelineBuilder> {
    return std::nullopt;
}

auto ENGINE_NS::graphics::RegisteredPipeline::build_compute_pipeline(ENGINE_NS::GraphicsEngine& /* unused */,
                                                                     VulkanDevice& /* unused */,
                                                                     GraphicsRegisteredPipelineDeletionQueue& /*unused*/)
    -> std::optional<ComputePipelineBuilder> {
    return std::nullopt;
}

auto ENGINE_NS::graphics::RegisteredPipeline::init_pipeline(GraphicsEngine& engine, VulkanDevice& device, VmaAllocator allocator) -> void {
    auto logger = ENGINE_NS::g_ENGINE->logger.get(ENGINE_NS::LogNamespaces::GRAPHICS);
    logger.get().debug("Creating registered pipeline \"{}\"", this->name());

    std::vector<DescriptorAllocatorGrowable::PoolSizeRatio> frame_sizes = {
      {.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,          .ratio = 3},
      {.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,         .ratio = 3},
      {.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         .ratio = 3},
      {.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, .ratio = 4},
    };

    pipeline_descriptor_allocator_.init(device, 1'000, frame_sizes);
    this->deletion_queue_.push(pipeline_descriptor_allocator_);

    engine::GraphicsRegisteredPipelineDeletionQueue initialisation_queue{};

    auto graphics_pipeline = this->build_graphics_pipeline(engine, device, initialisation_queue);
    if (graphics_pipeline.has_value()) {
        this->graphics_pipeline_ = graphics_pipeline.value().finish(device);
        this->deletion_queue_.push(this->graphics_pipeline_.value());
    }
    auto compute_pipeline = this->build_compute_pipeline(engine, device, initialisation_queue);
    if (compute_pipeline.has_value()) {
        this->compute_pipeline_ = compute_pipeline.value().finish(device);
        this->deletion_queue_.push(this->compute_pipeline_.value());
    }

    initialisation_queue.flush(device, allocator);
}

auto ENGINE_NS::graphics::RegisteredPipeline::destroy(VulkanDevice& device, VmaAllocator allocator) -> void {
    auto logger = ENGINE_NS::g_ENGINE->logger.get(ENGINE_NS::LogNamespaces::GRAPHICS);
    logger.get().debug("Destroying registered pipeline \"{}\"", this->name());
    this->destroy_(device, allocator);
    this->deletion_queue_.flush(device, allocator);
}

auto ENGINE_NS::graphics::RegisteredPipeline::record_graphics(VkCommandBuffer) -> void {
}

auto ENGINE_NS::graphics::RegisteredPipeline::record_compute(VkCommandBuffer) -> void {
}

auto ENGINE_NS::graphics::RegisteredPipeline::push_constants() -> GPUPushConstants {
    return GPUPushConstants{};
}

auto ENGINE_NS::graphics::RegisteredPipeline::dependencies() -> std::vector<std::string> {
    return {};
}

auto ENGINE_NS::graphics::RegisteredPipeline::destroy_(VulkanDevice&, VmaAllocator) -> void {
}
