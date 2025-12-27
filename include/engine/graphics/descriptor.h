#pragma once
#include "engine/graphics/vulkan.h"
#include "engine/meta_defines.h"

#include <vector>

#include <cstdint>
#include <span>
#include <vulkan/vulkan_core.h>

namespace ENGINE_NS {
    class DescriptorAllocator {
        public:
            struct PoolSizeRatio {
                    VkDescriptorType type = {};
                    float ratio           = 0.0;
            };

            auto init(VulkanDevice& device, std::uint32_t max_sets, std::span<PoolSizeRatio> ratios) -> void;
            auto clear_descriptors() -> void;
            auto destroy() -> void;

            auto allocate(VkDescriptorSetLayout layout) -> VkDescriptorSet;

            DescriptorAllocator() = default;
            DescriptorAllocator(DescriptorAllocator&& rhs) noexcept;
            auto operator=(DescriptorAllocator&& rhs) noexcept -> DescriptorAllocator&;

        private:
            VkDescriptorPool pool_ = VK_NULL_HANDLE;
            VulkanDevice* device_  = nullptr;
            bool moved_            = false;
    };
} // namespace ENGINE_NS
