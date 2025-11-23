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

    VulkanPhysicalDevice::choose(window_)
        .set_minimum_vulkan_version(Version(1, 3, 0))
        .set_required_features_12(features12)
        .set_required_features_13(features)
        .finish(vulkan_instance_);
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
