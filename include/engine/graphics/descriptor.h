#pragma once
#include "engine/graphics/vulkan.h"
#include "engine/meta_defines.h"

#include <cstdint>
#include <span>
#include <vector>
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

    class DescriptorAllocatorGrowable {
        public:
            struct PoolSizeRatio {
                    VkDescriptorType type = {};
                    float ratio           = 0.0;
            };

            auto init(VulkanDevice& device, std::uint32_t max_sets, std::span<PoolSizeRatio> ratios) -> void;
            auto clear_pools(VulkanDevice& device) -> void;
            auto destroy_pools(VulkanDevice& device) -> void;
            auto destroy_pools(VkDevice device) -> void;

            auto allocate(VulkanDevice& device, VkDescriptorSetLayout layout, void* pNext = nullptr) -> VkDescriptorSet;

            DescriptorAllocatorGrowable() = default;
            DescriptorAllocatorGrowable(DescriptorAllocatorGrowable&& rhs) noexcept;
            auto operator=(DescriptorAllocatorGrowable&& rhs) noexcept -> DescriptorAllocatorGrowable&;

        private:
            auto get_pool_(VulkanDevice& device) -> VkDescriptorPool;
            auto create_pool_(VulkanDevice& device, std::uint32_t set_count, std::span<PoolSizeRatio> pool_ratios) -> VkDescriptorPool;

            std::vector<PoolSizeRatio> ratios_{};
            std::vector<VkDescriptorPool> full_pools_{};
            std::vector<VkDescriptorPool> ready_pools_{};
            std::uint32_t sets_per_pool_ = 0;
            bool moved_                  = false;
    };
} // namespace ENGINE_NS
