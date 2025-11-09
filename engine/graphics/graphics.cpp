#include "engine/graphics/graphics.h"
#include "engine/engine.h"
#include "engine/logger.h"
// clang-format off
#include <vulkan/vulkan.h>
// clang-format on
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <Tracy/Tracy.hpp>
#include <Tracy/TracyVulkan.hpp>
#include <Tracy/common/TracySystem.hpp>

void ENGINE_NS::GraphicsEngine::initialise() {
    FrameMarkStart(StaticNames::GraphicsInit);
    auto logger = ENGINE_NS::g_ENGINE->logger.get(ENGINE_NS::LogNamespaces::GRAPHICS);
    logger.info("Creating graphics engine");

    logger.info("Initialising SDL");
    SDL_Init(SDL_INIT_VIDEO);
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN);

    window_                      = SDL_CreateWindow("Vulkan Engine", window_extent_.x, window_extent_.y, window_flags);

    logger.info("Initialising render thread");
    running_.store(true, std::memory_order_release);
    update_rate_   = std::chrono::milliseconds(9);
    render_thread_ = std::thread::thread(&ENGINE_NS::GraphicsEngine::draw_, this);

    initialised_   = true;
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
        this->running_.store(false, std::memory_order_release);
        this->render_thread_.join();
    }

    logger.info("Cleaning up SDL");
    {
        ZoneScoped;
        SDL_DestroyWindow(window_);
    }

    initialised_ = false;
    FrameMarkEnd(StaticNames::GraphicsDeinit);
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
