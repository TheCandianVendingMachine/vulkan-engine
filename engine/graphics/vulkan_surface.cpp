#include "engine/engine.h"
#include "engine/engine_utils.h"
#include "engine/graphics/vulkan.h"
#include "engine/logger.h"
// clang-format disable
#include <volk/volk.h>
// clang-format enable
#include <SDL3/SDL_vulkan.h>


ENGINE_NS::VulkanSurface::VulkanSurface(SDL_Window* window, VulkanInstance& instance) : instance_(&instance), window_(window) {
    if (!SDL_Vulkan_CreateSurface(window_, instance_->instance, nullptr, &surface_)) {
        ::ENGINE_NS::crash(ErrorCode::VULKAN_ERROR, __LINE__, __func__, __FILE__);
    }
    initialised_ = true;
}

auto ENGINE_NS::VulkanSurface::cleanup() -> void {
    auto logger = g_ENGINE->logger.get(engine::LogNamespaces::VULKAN);
    if (moved_ || (window_ == nullptr)) {
        logger.get().info("Surface has already been moved or cleaned up");
        return;
    }
    if (!initialised_) {
        logger.get().warning("Surface is not initialised and is trying to be cleaned up");
        return;
    }
    logger.get().info("Destroying surface");
    SDL_Vulkan_DestroySurface(instance_->instance, surface_, nullptr);
    window_   = nullptr;
    instance_ = nullptr;
    surface_  = VK_NULL_HANDLE;
}

auto ENGINE_NS::VulkanSurface::operator=(VulkanSurface&& rhs) noexcept -> VulkanSurface& {
    if (&rhs != this && !rhs.moved_) {
        this->instance_    = rhs.instance_;
        this->surface_     = rhs.surface_;
        this->window_      = rhs.window_;
        this->initialised_ = rhs.initialised_;
        rhs.moved_         = true;
    }
    return *this;
}
