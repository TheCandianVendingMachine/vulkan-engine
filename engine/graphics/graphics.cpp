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
        update_rate_   = std::chrono::milliseconds(9);
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
            if (frames_[idx].tracy_context) {
                TracyVkDestroy(frames_[idx].tracy_context);
            }
            vkDestroyCommandPool(device_.device, frames_[idx].command_pool, nullptr);
        }
    }
    swapchain_.cleanup();
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

auto ENGINE_NS::GraphicsEngine::current_frame() -> graphics::FrameData& {
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

    create_swapchain_();

    graphics_queue_ = device_.queues.at("graphics").get();
    transfer_queue_ = device_.queues.at("transfer").get();

    VkCommandPoolCreateInfo pool_info =
        command_pool_create_info(device_.queues.at("graphics").family, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    for (auto idx = 0; idx < graphics::FRAME_OVERLAP; idx++) {
        VK_CHECK(vkCreateCommandPool(device_.device, &pool_info, nullptr, &frames_[idx].command_pool));

        VkCommandBufferAllocateInfo buffer_alloc = command_buffer_allocate_info(frames_[idx].command_pool);
        VK_CHECK(vkAllocateCommandBuffers(device_.device, &buffer_alloc, &frames_[idx].main_command_buffer));
        frames_[idx].tracy_context =
            TracyVkContext(physical_device_.device, device_.device, device_.queues.at("graphics").get(), frames_[idx].main_command_buffer);
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
}

auto ENGINE_NS::GraphicsEngine::draw_() -> void {
    tracy::SetThreadName(StaticNames::GraphicsThreadName);

    auto last_frame = std::chrono::high_resolution_clock::now();
    while (running_.load(std::memory_order_acquire)) {
        FrameMarkStart(StaticNames::RenderLoop);
        ++frame_number_;

        auto frame_delta = std::chrono::high_resolution_clock::now() - last_frame;
        last_frame       = std::chrono::high_resolution_clock::now();
        if (frame_delta < this->update_rate_) {
            auto sleep = this->update_rate_ - frame_delta;
            std::this_thread::sleep_for(sleep);
        }
        FrameMarkEnd(StaticNames::RenderLoop);
    }
}
