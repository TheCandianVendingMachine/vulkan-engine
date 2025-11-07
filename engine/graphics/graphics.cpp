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

void ENGINE_NS::GraphicsEngine::initialise() {
    FrameMarkStart(StaticNames::GraphicsInit);
    auto logger = ENGINE_NS::g_ENGINE->logger.get(ENGINE_NS::LogNamespaces::GRAPHICS);
    logger.info("Creating graphics engine");

    logger.info("Initialising SDL");
    SDL_Init(SDL_INIT_VIDEO);
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN);

    window_                      = SDL_CreateWindow("Vulkan Engine", window_extent_.x, window_extent_.y, window_flags);

    initialised_                 = true;
    FrameMarkEnd(StaticNames::GraphicsInit);
}

void ENGINE_NS::GraphicsEngine::draw() {
    FrameMarkStart(StaticNames::GraphicsDraw);
    auto logger = ENGINE_NS::g_ENGINE->logger.get(ENGINE_NS::LogNamespaces::GRAPHICS);
    ++frame_number_;
    FrameMarkEnd(StaticNames::GraphicsDraw);
}

void ENGINE_NS::GraphicsEngine::cleanup() {
    FrameMarkStart(StaticNames::GraphicsDeinit);
    auto logger = ENGINE_NS::g_ENGINE->logger.get(ENGINE_NS::LogNamespaces::GRAPHICS);
    logger.info("Cleaning up graphics engine");

    logger.info("Cleaning up SDL");
    SDL_DestroyWindow(window_);

    initialised_ = false;
    FrameMarkEnd(StaticNames::GraphicsDeinit);
}
