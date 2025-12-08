#include "engine/graphics/vulkan.h"
// clang-format disable
#include <Volk/volk.h>
// clang-format enable
#include <vulkan/vulkan_core.h>

auto ENGINE_NS::transition_image(VkCommandBuffer cmd, VkImage image, VkImageLayout current_layout, VkImageLayout new_layout) -> void {
    VkImageMemoryBarrier2 image_barrier{};
    image_barrier.sType         = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    image_barrier.pNext         = nullptr;

    image_barrier.srcStageMask  = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    image_barrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
    image_barrier.dstStageMask  = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    image_barrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;

    image_barrier.oldLayout     = current_layout;
    image_barrier.newLayout     = new_layout;

    VkImageAspectFlags aspect_mask =
        (new_layout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
    image_barrier.subresourceRange = image_subresource_range(aspect_mask);
    image_barrier.image            = image;

    VkDependencyInfo dep_info{};
    dep_info.sType                   = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    dep_info.pNext                   = nullptr;

    dep_info.imageMemoryBarrierCount = 1;
    dep_info.pImageMemoryBarriers    = &image_barrier;

    vkCmdPipelineBarrier2(cmd, &dep_info);
}

auto ENGINE_NS::image_subresource_range(VkImageAspectFlags aspect_mask) -> VkImageSubresourceRange {
    VkImageSubresourceRange subImage{};
    subImage.aspectMask     = aspect_mask;
    subImage.baseMipLevel   = 0;
    subImage.levelCount     = VK_REMAINING_MIP_LEVELS;
    subImage.baseArrayLayer = 0;
    subImage.layerCount     = VK_REMAINING_ARRAY_LAYERS;

    return subImage;
}

auto ENGINE_NS::image_create_info(VkFormat format, VkImageUsageFlags usage_flags, VkExtent3D extent) -> VkImageCreateInfo {
    VkImageCreateInfo info = {};
    info.sType             = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    info.pNext             = nullptr;

    info.imageType         = VK_IMAGE_TYPE_2D;

    info.format            = format;
    info.extent            = extent;

    info.mipLevels         = 1;
    info.arrayLayers       = 1;

    // for MSAA. we will not be using it by default, so default it to 1 sample per pixel.
    info.samples           = VK_SAMPLE_COUNT_1_BIT;

    // optimal tiling, which means the image is stored on the best gpu format
    info.tiling            = VK_IMAGE_TILING_OPTIMAL;
    info.usage             = usage_flags;

    return info;
}

auto ENGINE_NS::image_view_create_info(VkFormat format, VkImage image, VkImageAspectFlags aspect_flags) -> VkImageViewCreateInfo {
    // build a image-view for the depth image to use for rendering
    VkImageViewCreateInfo info           = {};
    info.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    info.pNext                           = nullptr;

    info.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
    info.image                           = image;
    info.format                          = format;
    info.subresourceRange.baseMipLevel   = 0;
    info.subresourceRange.levelCount     = 1;
    info.subresourceRange.baseArrayLayer = 0;
    info.subresourceRange.layerCount     = 1;
    info.subresourceRange.aspectMask     = aspect_flags;

    return info;
}

auto ENGINE_NS::blit_image(VkCommandBuffer cmd, VkImage from, VkImage to, VkExtent2D src_size, VkExtent2D dst_size) -> void {
    VkImageBlit2 blit_region{.sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2, .pNext = nullptr};

    blit_region.srcOffsets[1].x               = src_size.width;
    blit_region.srcOffsets[1].y               = src_size.height;
    blit_region.srcOffsets[1].z               = 1;

    blit_region.dstOffsets[1].x               = dst_size.width;
    blit_region.dstOffsets[1].y               = dst_size.height;
    blit_region.dstOffsets[1].z               = 1;

    blit_region.srcSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    blit_region.srcSubresource.baseArrayLayer = 0;
    blit_region.srcSubresource.layerCount     = 1;
    blit_region.srcSubresource.mipLevel       = 0;

    blit_region.dstSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    blit_region.dstSubresource.baseArrayLayer = 0;
    blit_region.dstSubresource.layerCount     = 1;
    blit_region.dstSubresource.mipLevel       = 0;

    VkBlitImageInfo2 blit_info{.sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2, .pNext = nullptr};
    blit_info.dstImage       = to;
    blit_info.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    blit_info.srcImage       = from;
    blit_info.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    blit_info.filter         = VK_FILTER_LINEAR;
    blit_info.regionCount    = 1;
    blit_info.pRegions       = &blit_region;

    vkCmdBlitImage2(cmd, &blit_info);
}
