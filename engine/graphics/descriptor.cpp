#include "engine/graphics/descriptor.h"

#include "engine/engine.h"
#include "engine/graphics/util.h"
#include "engine/graphics/vulkan.h"
#include "engine/logger.h"

#include <Volk/volk.h>
#include <cstdint>
#include <type_traits>

#include <vulkan/vulkan_core.h>

auto ENGINE_NS::VulkanDescriptorLayoutBuilder::with_binding(std::uint32_t binding_idx, VkDescriptorType type)
    -> VulkanDescriptorLayoutBuilder& {
    VkDescriptorSetLayoutBinding binding{};
    binding.binding         = binding_idx;
    binding.descriptorCount = 1;
    binding.descriptorType  = type;
    bindings_.push_back(binding);
    return *this;
}

auto ENGINE_NS::VulkanDescriptorLayoutBuilder::build(VulkanDevice& device,
                                                     VkShaderStageFlags shader_stages,
                                                     void* next,
                                                     VkDescriptorSetLayoutCreateFlags flags) -> VulkanDescriptorSetLayout {
    for (auto& binding : bindings_) {
        binding.stageFlags |= shader_stages;
    }

    VkDescriptorSetLayoutCreateInfo info = {};
    info.sType                           = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    info.pNext                           = next;
    info.pBindings                       = bindings_.data();
    info.bindingCount                    = static_cast<std::uint32_t>(bindings_.size());
    info.flags                           = flags;

    return VulkanDescriptorSetLayout(info, device);
}

ENGINE_NS::VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(VkDescriptorSetLayoutCreateInfo create_info, VulkanDevice& device) {
    VK_CHECK(vkCreateDescriptorSetLayout(device.device, &create_info, nullptr, &set_));
    initialised_ = true;
}

ENGINE_NS::VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(const VulkanDescriptorSetLayout& rhs) : set_(rhs.set_), initialised_(true) {
}

ENGINE_NS::VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(VulkanDescriptorSetLayout&& rhs) noexcept :
    set_(std::move(rhs.set_)), initialised_(true) {
}

auto ENGINE_NS::VulkanDescriptorSetLayout::build() -> VulkanDescriptorLayoutBuilder {
    return VulkanDescriptorLayoutBuilder{};
}

auto ENGINE_NS::VulkanDescriptorSetLayout::operator=(const VulkanDescriptorSetLayout& rhs) -> VulkanDescriptorSetLayout& {
    if (&rhs != this) {
        set_         = rhs.set_;
        initialised_ = rhs.initialised_;
    }
    return *this;
}

auto ENGINE_NS::VulkanDescriptorSetLayout::operator=(VulkanDescriptorSetLayout&& rhs) noexcept -> VulkanDescriptorSetLayout& {
    if (&rhs != this) {
        set_         = std::move(rhs.set_);
        initialised_ = std::move(rhs.initialised_);

        rhs.moved_   = true;
    }
    return *this;
}


void ENGINE_NS::DescriptorAllocator::init(VulkanDevice& device, std::uint32_t max_sets, std::span<PoolSizeRatio> ratios) {
    auto& logger = g_ENGINE->logger.get(engine::LogNamespaces::VULKAN);
    if (moved_) {
        logger.info("Descriptor set allocator requesting initialisation has been moved!");
        return;
    }
    device_ = &device;
    std::vector<VkDescriptorPoolSize> sizes{};
    for (auto& ratio : ratios) {
        sizes.push_back(VkDescriptorPoolSize{.type = ratio.type, .descriptorCount = std::uint32_t(ratio.ratio * max_sets)});
    }

    VkDescriptorPoolCreateInfo pool_info{};
    pool_info.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags         = 0;
    pool_info.poolSizeCount = static_cast<std::uint32_t>(sizes.size());
    pool_info.pPoolSizes    = sizes.data();
    pool_info.maxSets       = max_sets;

    VK_CHECK(vkCreateDescriptorPool(device.device, &pool_info, nullptr, &pool_));
}

void ENGINE_NS::DescriptorAllocator::clear_descriptors() {
    auto& logger = g_ENGINE->logger.get(engine::LogNamespaces::VULKAN);
    if (moved_) {
        logger.info("Descriptor set allocator clearing descriptors has been moved!");
        return;
    }
    vkResetDescriptorPool(device_->device, pool_, 0);
}

void ENGINE_NS::DescriptorAllocator::destroy() {
    auto& logger = g_ENGINE->logger.get(engine::LogNamespaces::VULKAN);
    if (moved_) {
        logger.info("Descriptor set allocator trying to be destroy has been moved!");
        return;
    }
    vkDestroyDescriptorPool(device_->device, pool_, nullptr);
    device_ = nullptr;
    pool_   = VK_NULL_HANDLE;
}

auto ENGINE_NS::DescriptorAllocator::allocate(VkDescriptorSetLayout layout) -> VkDescriptorSet {
    auto& logger = g_ENGINE->logger.get(engine::LogNamespaces::VULKAN);
    if (moved_) {
        logger.info("Descriptor set allocator trying to be allocate has been moved!");
        return VK_NULL_HANDLE;
    }
    VkDescriptorSetAllocateInfo alloc_info{};
    alloc_info.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.pNext              = nullptr;
    alloc_info.descriptorPool     = pool_;
    alloc_info.descriptorSetCount = 1;
    alloc_info.pSetLayouts        = &layout;

    VkDescriptorSet ds            = VK_NULL_HANDLE;
    VK_CHECK(vkAllocateDescriptorSets(device_->device, &alloc_info, &ds));
    return ds;
}

ENGINE_NS::DescriptorAllocator::DescriptorAllocator(DescriptorAllocator&& rhs) noexcept :
    pool_(std::move(rhs.pool_)), device_(std::move(rhs.device_)) {
    rhs.moved_ = true;
}

auto ENGINE_NS::DescriptorAllocator::operator=(DescriptorAllocator&& rhs) noexcept -> ENGINE_NS::DescriptorAllocator& {
    if (&rhs != this && !rhs.moved_) {
        pool_      = std::move(rhs.pool_);
        device_    = std::move(rhs.device_);
        rhs.moved_ = true;
    }
    return *this;
}
