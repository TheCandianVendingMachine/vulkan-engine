#include "engine/graphics/graphics.h"
#include "engine/engine.h"
#include "engine/graphics/vulkan.h"
#include "engine/logger.h"
// clang-format off
#include <volk/volk.h>
// clang-format on
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <Tracy/Tracy.hpp>
#include <Tracy/TracyVulkan.hpp>
#include <Tracy/common/TracySystem.hpp>
#include <chrono>
#include <engine/rwlock.h>
#include <vulkan/vulkan_core.h>

void ENGINE_NS::GraphicsEngine::initialise() {
    FrameMarkStart(StaticNames::GraphicsInit);
    auto logger = ENGINE_NS::g_ENGINE->logger.get(ENGINE_NS::LogNamespaces::GRAPHICS);
    logger.info("Creating graphics engine");

    logger.info("Initialising SDL");
    SDL_Init(SDL_INIT_VIDEO);
    {
        ZoneScoped;
        SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN);

        window_                      = SDL_CreateWindow("Vulkan Engine", window_extent_.x, window_extent_.y, window_flags);
    }

    logger.info("Initialising Vulkan");
    init_vulkan_();

    logger.info("Initialising render thread");
    {
        ZoneScoped;
        running_.store(true, std::memory_order_release);
        update_rate_   = std::chrono::milliseconds(8);
        render_thread_ = std::thread::thread(&ENGINE_NS::GraphicsEngine::draw_, this);
    }

    initialised_ = true;
    FrameMarkEnd(StaticNames::GraphicsInit);
}

void ENGINE_NS::GraphicsEngine::draw() {
    FrameMarkStart(StaticNames::GraphicsDraw);
    auto logger = ENGINE_NS::g_ENGINE->logger.get(ENGINE_NS::LogNamespaces::GRAPHICS);
    FrameMarkEnd(StaticNames::GraphicsDraw);
}

void ENGINE_NS::GraphicsEngine::cleanup() {
    FrameMarkStart(StaticNames::GraphicsDeinit);
    auto logger = ENGINE_NS::g_ENGINE->logger.get(ENGINE_NS::LogNamespaces::GRAPHICS);
    logger.info("Cleaning up graphics engine");

    logger.info("Stopping render thread");
    {
        ZoneScoped;
        if (this->running_.load(std::memory_order_acquire)) {
            this->running_.store(false, std::memory_order_release);
            this->render_thread_.join();
        }
    }


    logger.info("Cleaning up Vulkan");
    if (device_.device != VK_NULL_HANDLE) {
        vkDeviceWaitIdle(device_.device);
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
    deletion_queue_.flush(device_, allocator_);
    swapchain_.cleanup();
    vmaDestroyAllocator(allocator_);
    device_.cleanup();
    surface_.cleanup();
    vulkan_instance_.cleanup();

    logger.info("Cleaning up SDL");
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

auto ENGINE_NS::GraphicsEngine::init_vulkan_() -> void {
    ZoneScoped;
    VK_CHECK(volkInitialize());
    vulkan_instance_ = VulkanInstance::build()
                           .engine_name(ENGINE_NAME_STR)
                           .engine_version(Version(1, 0, 0))
                           .game_name("Shooter")
                           .game_version(Version(1, 0, 0))
                           .with_validation_layers(true)
                           .finish();

    surface_ = VulkanSurface(window_, vulkan_instance_);

    // vulkan 1.3 features
    VkPhysicalDeviceVulkan13Features features{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES};
    features.dynamicRendering = true;
    features.synchronization2 = true;

    // vulkan 1.2 features
    VkPhysicalDeviceVulkan12Features features12{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES};
    features12.bufferDeviceAddress = true;
    features12.descriptorIndexing  = true;

    physical_device_               = VulkanPhysicalDevice::choose(window_)
                           .set_minimum_vulkan_version(Version(1, 3, 0))
                           .set_required_features_12(features12)
                           .set_required_features_13(features)
                           .with_extension(VK_KHR_SWAPCHAIN_EXTENSION_NAME)
                           .finish(vulkan_instance_);

    device_ = VulkanDevice::build()
                  .request_queue("graphics", VulkanQueueType::GRAPHICS | VulkanQueueType::TRANSFER | VulkanQueueType::COMPUTE)
                  .request_queue("transfer", VulkanQueueType::TRANSFER)
                  .finish(physical_device_);


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

    create_swapchain_();

    graphics_queue_ = device_.queues.at("graphics").get();
    transfer_queue_ = device_.queues.at("transfer").get();

    VkCommandPoolCreateInfo pool_info =
        command_pool_create_info(device_.queues.at("graphics").family, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    VkSemaphoreCreateInfo semaphore_info = semaphore_create_info(0);
    VkFenceCreateInfo fence_info         = fence_create_info(VK_FENCE_CREATE_SIGNALED_BIT);


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

auto ENGINE_NS::GraphicsEngine::draw_background_(VkCommandBuffer cmd) -> void {
    VkClearColorValue clear_value;
    float flash = std::abs(std::sin(frame_number_ / 120.f));
    clear_value = {
      {0.f, 0.f, flash, 1.f}
    };

    VkImageSubresourceRange clear_range = image_subresource_range(VK_IMAGE_ASPECT_COLOR_BIT);
    vkCmdClearColorImage(cmd, draw_image_.image, VK_IMAGE_LAYOUT_GENERAL, &clear_value, 1, &clear_range);
}

auto ENGINE_NS::GraphicsEngine::draw_() -> void {
    tracy::SetThreadName(StaticNames::GraphicsThreadName);
    constexpr std::uint32_t TIMEOUT = 250'000'000;

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

            std::uint32_t swapchain_image_index = 0;
            VK_CHECK(vkAcquireNextImageKHR(device_.device, swapchain_.swapchain, TIMEOUT, frame.get().swapchain_semaphore_, VK_NULL_HANDLE,
                                           &swapchain_image_index));

            VkCommandBuffer cmd = frame.get().main_command_buffer;
            VK_CHECK(vkResetCommandBuffer(cmd, 0));
            VkCommandBufferBeginInfo cmd_begin_info = command_buffer_begin_info(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

            VkExtent2D draw_extent                  = {draw_image_.extent.width, draw_image_.extent.height};

            VK_CHECK(vkBeginCommandBuffer(cmd, &cmd_begin_info));
            TracyVkZone(frame.get().tracy_context_, cmd, StaticNames::MainCommandBufferName);

            transition_image(cmd, draw_image_.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

            draw_background_(cmd);

            transition_image(cmd, draw_image_.image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
            transition_image(cmd, swapchain_.images[swapchain_image_index], VK_IMAGE_LAYOUT_UNDEFINED,
                             VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

            blit_image(cmd, draw_image_.image, swapchain_.images[swapchain_image_index], draw_extent, swapchain_.extent);


            transition_image(cmd, swapchain_.images[swapchain_image_index], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                             VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

            TracyVkCollect(frame.get().tracy_context_, cmd);
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
