#include "engine/engine.h"
#include "engine/graphics/vulkan.h"
#include "engine/logger.h"
// clang-format disable
#include <Volk/volk.h>
// clang-format enable
#include <Tracy/Tracy.hpp>
#include <cstdint>
#include <type_traits>
#include <vector>

auto ENGINE_NS::VulkanSwapchain::build() -> VulkanSwapchainBuilder {
    return VulkanSwapchainBuilder{};
}

auto ENGINE_NS::VulkanSwapchain::operator=(VulkanSwapchain&& rhs) -> VulkanSwapchain& {
    if (!rhs.moved_ && this != &rhs) {
        this->device_    = std::move(rhs.device_);
        this->extent_    = std::move(rhs.extent_);
        this->format_    = std::move(rhs.format_);
        this->images_    = std::move(rhs.images_);
        this->swapchain_ = std::move(rhs.swapchain_);
        this->views_     = std::move(rhs.views_);
        rhs.moved_       = true;
    }
    return *this;
}

auto ENGINE_NS::VulkanSwapchain::cleanup() -> void {
    if (moved_) {
        return;
    }
    vkDestroySwapchainKHR(device_->device, swapchain_, nullptr);
    for (auto& view : views_) {
        vkDestroyImageView(device_->device, view, nullptr);
    }
}

ENGINE_NS::VulkanSwapchain::VulkanSwapchain(VkSwapchainCreateInfoKHR create_info, VulkanDevice& device) {
    ZoneScoped;
    device_ = &device;
    format_ = create_info.imageFormat;
    extent_ = create_info.imageExtent;
    VK_CHECK(vkCreateSwapchainKHR(device.device, &create_info, nullptr, &swapchain_));

    std::uint32_t image_count = 0;
    vkGetSwapchainImagesKHR(device.device, swapchain_, &image_count, nullptr);
    images_.resize(image_count);
    views_.resize(image_count);
    vkGetSwapchainImagesKHR(device.device, swapchain_, &image_count, images_.data());

    for (std::uint32_t idx = 0; idx < image_count; idx++) {
        VkImageViewCreateInfo view_info{};
        view_info.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        view_info.image                           = images_[idx];
        view_info.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
        view_info.format                          = format_;

        view_info.components.r                    = VK_COMPONENT_SWIZZLE_IDENTITY;
        view_info.components.g                    = VK_COMPONENT_SWIZZLE_IDENTITY;
        view_info.components.b                    = VK_COMPONENT_SWIZZLE_IDENTITY;
        view_info.components.a                    = VK_COMPONENT_SWIZZLE_IDENTITY;

        view_info.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        view_info.subresourceRange.baseMipLevel   = 0;
        view_info.subresourceRange.levelCount     = 1;
        view_info.subresourceRange.baseArrayLayer = 0;
        view_info.subresourceRange.layerCount     = 1;

        VK_CHECK(vkCreateImageView(device.device, &view_info, nullptr, &views_[idx]));
    }
}

auto ENGINE_NS::VulkanSwapchainBuilder::finish(VulkanPhysicalDevice& physical_device, VulkanSurface& surface, VulkanDevice& device)
    -> VulkanSwapchain {
    VkSurfaceCapabilitiesKHR capabilities{};
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device.device, surface.surface, &capabilities);

    VkSwapchainCreateInfoKHR create_info{};
    create_info.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface          = surface.surface;
    create_info.imageFormat      = desired_format_.format;
    create_info.imageColorSpace  = desired_format_.colorSpace;
    create_info.imageExtent      = extent_;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage       = image_usage_flags_;
    create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    create_info.compositeAlpha   = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.presentMode      = present_mode_;
    create_info.clipped          = VK_TRUE;
    create_info.preTransform     = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;

    switch (present_mode_) {
        case VkPresentModeKHR::VK_PRESENT_MODE_MAILBOX_KHR:
            create_info.minImageCount = (capabilities.minImageCount + capabilities.maxImageCount) / 2;
            break;
        default:
            create_info.minImageCount = capabilities.minImageCount;
            break;
    }

    return VulkanSwapchain(create_info, device);
}

auto ENGINE_NS::VulkanSwapchainBuilder::set_desired_format(VkSurfaceFormatKHR format) -> VulkanSwapchainBuilder& {
    desired_format_ = format;
    return *this;
}

auto ENGINE_NS::VulkanSwapchainBuilder::set_present_mode(VkPresentModeKHR present) -> VulkanSwapchainBuilder& {
    present_mode_ = present;
    return *this;
}

auto ENGINE_NS::VulkanSwapchainBuilder::set_extent(VkExtent2D extent) -> VulkanSwapchainBuilder& {
    extent_ = extent;
    return *this;
}

auto ENGINE_NS::VulkanSwapchainBuilder::set_extent(::linalg::Vector2<unsigned int> extent) -> VulkanSwapchainBuilder& {
    return set_extent(VkExtent2D{.width = extent.x, .height = extent.y});
}

auto ENGINE_NS::VulkanSwapchainBuilder::add_image_usage_flags(VkImageUsageFlags flags) -> VulkanSwapchainBuilder& {
    image_usage_flags_ |= flags;
    return *this;
}
