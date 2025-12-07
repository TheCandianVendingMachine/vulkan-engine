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

auto ENGINE_NS::VulkanSwapchain::operator=(VulkanSwapchain&& rhs) noexcept -> VulkanSwapchain& {
    if (!rhs.moved_ && this != &rhs) {
        this->device_      = std::move(rhs.device_);
        this->extent_      = std::move(rhs.extent_);
        this->format_      = std::move(rhs.format_);
        this->images_      = std::move(rhs.images_);
        this->swapchain_   = std::move(rhs.swapchain_);
        this->views_       = std::move(rhs.views_);
        this->semaphores_  = std::move(rhs.semaphores_);
        this->initialised_ = std::move(rhs.initialised_);
        rhs.moved_         = true;
    }
    return *this;
}

auto ENGINE_NS::VulkanSwapchain::cleanup() -> void {
    auto& logger = g_ENGINE->logger.get(engine::LogNamespaces::VULKAN);
    if (moved_) {
        logger.info("Swapchain has already been moved or cleaned up");
        return;
    }
    if (!initialised_) {
        logger.warning("Swapchain is not initialised and is trying to be cleaned up");
        return;
    }
    for (auto& semaphore : semaphores_) {
        vkDestroySemaphore(device_->device, semaphore, nullptr);
    }
    vkDestroySwapchainKHR(device_->device, swapchain_, nullptr);
    for (auto& view : views_) {
        vkDestroyImageView(device_->device, view, nullptr);
    }
    initialised_ = false;
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
    semaphores_.resize(image_count);
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

        VkSemaphoreCreateInfo semaphore_info = semaphore_create_info(0);
        VK_CHECK(vkCreateSemaphore(device.device, &semaphore_info, nullptr, &semaphores[idx]));
    }
    initialised_ = true;
}

auto ENGINE_NS::VulkanSwapchainBuilder::finish(VulkanPhysicalDevice& physical_device, VulkanSurface& surface, VulkanDevice& device)
    -> VulkanSwapchain {
    auto& logger = g_ENGINE->logger.get(engine::LogNamespaces::VULKAN);
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

    std::uint32_t format_count = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device.device, surface.surface, &format_count, nullptr);
    std::vector<VkSurfaceFormatKHR> formats(format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device.device, surface.surface, &format_count, formats.data());
    bool does_format_exist                = false;
    bool does_color_space_exist           = false;
    bool does_combination_exist           = false;

    std::size_t idx                       = 0;
    std::size_t first_similar_format      = 0;
    std::size_t first_similar_color_space = 0;

    for (auto& format : formats) {
        bool format_equals       = format.format == create_info.imageFormat;
        bool colour_space_equals = format.colorSpace == create_info.imageColorSpace;
        if (format_equals && !does_format_exist) {
            first_similar_format = idx;
            does_format_exist    = true;
        }
        if (colour_space_equals && !does_color_space_exist) {
            first_similar_color_space = idx;
            does_color_space_exist    = true;
        }
        if (format_equals && colour_space_equals) {
            does_combination_exist = true;
            break;
        }
        idx++;
    }

    if (!does_combination_exist) {
        assert(!does_format_exist || !does_color_space_exist);
        if (!does_format_exist && !does_color_space_exist) {
            logger.warning("Requested format and color space does not exist on device. Falling back to a valid format");
            if (formats.size() > 0) {
                create_info.imageFormat     = formats[0].format;
                create_info.imageColorSpace = formats[0].colorSpace;
            } else {
                crash(ErrorCode::VULKAN_ERROR, __LINE__, __FUNCTION__, __FILE__, "No formats exist on the device for this surface");
            }
        } else if (does_format_exist && !does_color_space_exist) {
            logger.warning(
                "Requested format exists on device, however the desired colour space does not. Falling back to a valid color space");
            create_info.imageColorSpace = formats[first_similar_color_space].colorSpace;
        } else if (!does_format_exist && does_color_space_exist) {
            logger.warning("Requested color space exists on device, however the desired format does not. Falling back to a valid format");
            create_info.imageFormat = formats[first_similar_format].format;
        }
    } else {
        logger.debug("Requested format and color space exists.");
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
