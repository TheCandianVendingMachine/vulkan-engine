#include "engine/engine.h"
#include "engine/graphics/vulkan.h"
#include "engine/logger.h"
// clang-format disable
#include <Volk/volk.h>
// clang-format enable
#include <SDL3/SDL_vulkan.h>
#include <type_traits>

ENGINE_NS::VulkanSurface::VulkanSurface(SDL_Window* window, VulkanInstance& instance) : window_(window), instance_(&instance) {
    if (!SDL_Vulkan_CreateSurface(window_, instance_->instance, nullptr, &surface_)) {
        ::ENGINE_NS::crash(ErrorCode::VULKAN_ERROR, __LINE__, __func__, __FILE__);
    }
}

auto ENGINE_NS::VulkanSurface::cleanup() -> void {
    auto& logger = g_ENGINE->logger.get(engine::LogNamespaces::VULKAN);
    if (moved_ || !window_) {
        logger.info("Surface has already been moved or cleaned up");
        return;
    }
    logger.info("Destroying surface");
    SDL_Vulkan_DestroySurface(instance_->instance, surface_, nullptr);
    window_   = nullptr;
    instance_ = nullptr;
    surface_  = VK_NULL_HANDLE;
}

auto ENGINE_NS::VulkanSurface::operator=(VulkanSurface&& rhs) -> VulkanSurface& {
    if (&rhs != this && !rhs.moved_) {
        this->instance_ = std::move(rhs.instance_);
        this->surface_  = std::move(rhs.surface_);
        this->window_   = std::move(rhs.window_);
        rhs.moved_      = true;
    }
    return *this;
}
