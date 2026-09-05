#include "engine/graphics/descriptor.h"

#include "engine/engine.h"
#include "engine/graphics/util.h"
#include "engine/graphics/vulkan.h"
#include "engine/logger.h"

#include <Volk/volk.h>
#include <engine/graphics/types.h>
#include <engine/meta_defines.h>
#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <deque>
#include <span>
#include <utility>
#include <vector>


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

    VkDescriptorSetLayoutCreateInfo info{};
    info.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    info.pNext        = next;
    info.pBindings    = bindings_.data();
    info.bindingCount = static_cast<std::uint32_t>(bindings_.size());
    info.flags        = flags;

    return {info, device};
}

ENGINE_NS::VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(VkDescriptorSetLayoutCreateInfo create_info, VulkanDevice& device) {
    VK_CHECK(vkCreateDescriptorSetLayout(device.device, &create_info, nullptr, &set_));
    initialised_ = true;
}

ENGINE_NS::VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(const VulkanDescriptorSetLayout& rhs) : set_(rhs.set_), initialised_(true) {
}

ENGINE_NS::VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(VulkanDescriptorSetLayout&& rhs) noexcept :
    set_(rhs.set_), initialised_(true) {
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

        rhs.moved_ = true;
    }
    return *this;
}


void ENGINE_NS::DescriptorAllocator::init(VulkanDevice& device, std::uint32_t max_sets, std::span<PoolSizeRatio> ratios) {
    auto logger = g_ENGINE->logger.get(engine::LogNamespaces::VULKAN);
    if (moved_) {
        logger.get().info("Descriptor set allocator requesting initialisation has been moved!");
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
    auto logger = g_ENGINE->logger.get(engine::LogNamespaces::VULKAN);
    if (moved_) {
        logger.get().info("Descriptor set allocator clearing descriptors has been moved!");
        return;
    }
    vkResetDescriptorPool(device_->device, pool_, 0);
}

void ENGINE_NS::DescriptorAllocator::destroy() {
    auto logger = g_ENGINE->logger.get(engine::LogNamespaces::VULKAN);
    if (moved_) {
        logger.get().info("Descriptor set allocator trying to be destroy has been moved!");
        return;
    }
    vkDestroyDescriptorPool(device_->device, pool_, nullptr);
    device_ = nullptr;
    pool_   = VK_NULL_HANDLE;
}

auto ENGINE_NS::DescriptorAllocator::allocate(VkDescriptorSetLayout layout) -> VkDescriptorSet {
    auto logger = g_ENGINE->logger.get(engine::LogNamespaces::VULKAN);
    if (moved_) {
        logger.get().info("Descriptor set allocator trying to be allocate has been moved!");
        return VK_NULL_HANDLE;
    }
    VkDescriptorSetAllocateInfo alloc_info{};
    alloc_info.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.pNext              = nullptr;
    alloc_info.descriptorPool     = pool_;
    alloc_info.descriptorSetCount = 1;
    alloc_info.pSetLayouts        = &layout;

    VkDescriptorSet ds = VK_NULL_HANDLE;
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

auto ENGINE_NS::DescriptorAllocatorGrowable::init(VulkanDevice& device, std::uint32_t max_sets, std::span<PoolSizeRatio> ratios) -> void {
    ratios_.clear();
    for (auto& r : ratios) {
        ratios_.push_back(r);
    }

    VkDescriptorPool new_pool = create_pool_(device, max_sets, ratios);
    sets_per_pool_            = static_cast<std::uint32_t>(max_sets * 1.5);

    ready_pools_.push_back(new_pool);
}

auto ENGINE_NS::DescriptorAllocatorGrowable::clear_pools(VulkanDevice& device) -> void {
    if (this->moved_) {
        return;
    }

    for (auto& pool : ready_pools_) {
        vkResetDescriptorPool(device.device, pool, 0);
    }
    for (auto& pool : full_pools_) {
        vkResetDescriptorPool(device.device, pool, 0);
    }
    ready_pools_.insert(ready_pools_.end(), full_pools_.begin(), full_pools_.end());
    full_pools_.clear();
}

auto ENGINE_NS::DescriptorAllocatorGrowable::destroy_pools(VulkanDevice& device) -> void {
    destroy_pools(device.device);
}

auto ENGINE_NS::DescriptorAllocatorGrowable::destroy_pools(VkDevice device) -> void {
    if (this->moved_) {
        return;
    }

    for (auto& pool : ready_pools_) {
        vkDestroyDescriptorPool(device, pool, nullptr);
    }
    for (auto& pool : full_pools_) {
        vkDestroyDescriptorPool(device, pool, nullptr);
    }
    ready_pools_.clear();
    full_pools_.clear();
}

auto ENGINE_NS::DescriptorAllocatorGrowable::allocate(VulkanDevice& device, VkDescriptorSetLayout layout, void* pNext) -> VkDescriptorSet {
    if (this->moved_) {
        return VK_NULL_HANDLE;
    }

    VkDescriptorPool pool_to_use = get_pool_(device);

    VkDescriptorSetAllocateInfo allocate_info{};
    allocate_info.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocate_info.pNext              = pNext;
    allocate_info.descriptorPool     = pool_to_use;
    allocate_info.descriptorSetCount = 1;
    allocate_info.pSetLayouts        = &layout;

    VkDescriptorSet descriptor_set;
    VkResult result = vkAllocateDescriptorSets(device.device, &allocate_info, &descriptor_set);

    if (result == VK_ERROR_OUT_OF_POOL_MEMORY || result == VK_ERROR_FRAGMENTED_POOL) {
        full_pools_.push_back(pool_to_use);

        pool_to_use                  = get_pool_(device);
        allocate_info.descriptorPool = pool_to_use;

        VK_CHECK(vkAllocateDescriptorSets(device.device, &allocate_info, &descriptor_set));
    }

    ready_pools_.push_back(pool_to_use);
    return descriptor_set;
}

ENGINE_NS::DescriptorAllocatorGrowable::DescriptorAllocatorGrowable(DescriptorAllocatorGrowable&& rhs) noexcept :
    ratios_(std::move(rhs.ratios_)), full_pools_(std::move(rhs.full_pools_)), ready_pools_(std::move(rhs.ready_pools_)),
    sets_per_pool_(std::move(rhs.sets_per_pool_)) {
    rhs.moved_ = true;
}

auto ENGINE_NS::DescriptorAllocatorGrowable::operator=(DescriptorAllocatorGrowable&& rhs) noexcept -> DescriptorAllocatorGrowable& {
    if (&rhs != this) {
        this->ratios_        = std::move(rhs.ratios_);
        this->full_pools_    = std::move(rhs.full_pools_);
        this->ready_pools_   = std::move(rhs.ready_pools_);
        this->sets_per_pool_ = std::move(rhs.sets_per_pool_);
        rhs.moved_           = true;
    }
    return *this;
}

ENGINE_NS::DescriptorAllocatorGrowable::DescriptorAllocatorGrowable(const DescriptorAllocatorGrowable& rhs) :
    ratios_(rhs.ratios_), full_pools_(rhs.full_pools_), ready_pools_(rhs.ready_pools_), sets_per_pool_(rhs.sets_per_pool_) {
}

auto ENGINE_NS::DescriptorAllocatorGrowable::operator=(const DescriptorAllocatorGrowable& rhs) -> DescriptorAllocatorGrowable& {
    if (&rhs != this) {
        this->ratios_        = rhs.ratios_;
        this->full_pools_    = rhs.full_pools_;
        this->ready_pools_   = rhs.ready_pools_;
        this->sets_per_pool_ = rhs.sets_per_pool_;
    }
    return *this;
}

auto ENGINE_NS::DescriptorAllocatorGrowable::get_pool_(VulkanDevice& device) -> VkDescriptorPool {
    VkDescriptorPool new_pool{};
    if (!ready_pools_.empty()) {
        new_pool = ready_pools_.back();
        ready_pools_.pop_back();
    } else {
        new_pool       = create_pool_(device, sets_per_pool_, ratios_);
        sets_per_pool_ = std::min(4'096u, static_cast<std::uint32_t>(sets_per_pool_ * 1.5));
    }

    return new_pool;
}

auto ENGINE_NS::DescriptorAllocatorGrowable::create_pool_(VulkanDevice& device,
                                                          std::uint32_t set_count,
                                                          std::span<PoolSizeRatio> pool_ratios) -> VkDescriptorPool {
    std::vector<VkDescriptorPoolSize> pool_sizes{};
    for (auto& ratio : pool_ratios) {
        pool_sizes.push_back(VkDescriptorPoolSize{.type = ratio.type, .descriptorCount = std::uint32_t(ratio.ratio * set_count)});
    }

    VkDescriptorPoolCreateInfo pool_info{};
    pool_info.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags         = 0;
    pool_info.maxSets       = set_count;
    pool_info.poolSizeCount = std::uint32_t(pool_sizes.size());
    pool_info.pPoolSizes    = pool_sizes.data();

    VkDescriptorPool pool;
    vkCreateDescriptorPool(device.device, &pool_info, nullptr, &pool);
    return pool;
}

auto ENGINE_NS::DescriptorWriter::write_image(Binding binding,
                                              VkImageView image,
                                              VkSampler sampler,
                                              VkImageLayout layout,
                                              VkDescriptorType type) -> DescriptorWriter& {
    VkDescriptorImageInfo& image_info =
        image_infos.emplace_back(VkDescriptorImageInfo{.sampler = sampler, .imageView = image, .imageLayout = layout});

    VkWriteDescriptorSet write{};
    write.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstBinding      = static_cast<std::uint32_t>(binding);
    write.dstSet          = VK_NULL_HANDLE;
    write.descriptorCount = 1;
    write.descriptorType  = type;
    write.pImageInfo      = &image_info;

    writes.push_back(write);
    return *this;
}

auto ENGINE_NS::DescriptorWriter::write_buffer(Binding binding,
                                               VkBuffer buffer,
                                               std::size_t size,
                                               std::size_t offset,
                                               VkDescriptorType type) -> DescriptorWriter& {
    VkDescriptorBufferInfo& buffer_info =
        buffer_infos.emplace_back(VkDescriptorBufferInfo{.buffer = buffer, .offset = offset, .range = size});

    VkWriteDescriptorSet write{};
    write.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstBinding      = static_cast<std::uint32_t>(binding);
    write.dstSet          = VK_NULL_HANDLE;
    write.descriptorCount = 1;
    write.descriptorType  = type;
    write.pBufferInfo     = &buffer_info;

    writes.push_back(write);
    return *this;
}

auto ENGINE_NS::DescriptorWriter::clear() -> void {
    image_infos.clear();
    writes.clear();
    buffer_infos.clear();
}

auto ENGINE_NS::DescriptorWriter::update_set(VulkanDevice& device, VkDescriptorSet set) -> void {
    for (auto& write : writes) {
        write.dstSet = set;
    }
    vkUpdateDescriptorSets(device.device, static_cast<std::uint32_t>(writes.size()), writes.data(), 0, nullptr);
}
