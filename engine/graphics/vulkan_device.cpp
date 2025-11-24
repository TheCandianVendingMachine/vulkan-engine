#include "engine/engine.h"
#include "engine/graphics/vulkan.h"
#include "engine/logger.h"
// clang-format disable
#include <Volk/volk.h>
// clang-format enable
#include <Tracy/Tracy.hpp>
#include <cstdint>
#include <robin_map.h>
#include <robin_set.h>
#include <type_traits>
#include <utility>
#include <vector>

auto ENGINE_NS::VulkanDevice::build() -> VulkanDeviceBuilder {
    return VulkanDeviceBuilder();
}

auto ENGINE_NS::VulkanDevice::cleanup() -> void {
    auto& logger = g_ENGINE->logger.get(engine::LogNamespaces::VULKAN);
    if (this->moved_) {
        logger.info("Device has already been moved or destroyed");
        return;
    }
    if (!this->initialised_) {
        logger.warning("Attempting to cleanup unitialised device");
    }
    logger.info("Destroying device");
    vkDestroyDevice(device_, nullptr);
    this->device_      = VK_NULL_HANDLE;
    this->moved_       = true;
    this->initialised_ = false;
}

auto ENGINE_NS::VulkanDevice::operator=(VulkanDevice&& rhs) noexcept -> VulkanDevice& {
    if (!rhs.moved_ && &rhs != this) {
        this->device_      = std::move(rhs.device_);
        this->queues_      = std::move(rhs.queues_);
        this->initialised_ = std::move(rhs.initialised_);
        rhs.moved_         = true;
    }
    return *this;
}

ENGINE_NS::VulkanDevice::VulkanDevice(tsl::robin_map<std::string, VulkanQueue>&& queues, VulkanPhysicalDevice& physical_device,
                                      VkDeviceCreateInfo create_info) : queues_(queues) {
    ZoneScoped;

    VK_CHECK(vkCreateDevice(physical_device.device, &create_info, nullptr, &device_));

    for (auto& [_, queue] : queues_) {
        // hate this const cast, but i cant get a mutable iterator otherwise
        const_cast<VulkanQueue&>(queue).device_ = this;
        if (const_cast<VulkanQueue&>(queue).get() == VK_NULL_HANDLE) {
            ::ENGINE_NS::crash(ErrorCode::VULKAN_ERROR, __LINE__, __func__, __FILE__, "Could not allocate queue");
        }
    }

    initialised_ = true;
}

auto ENGINE_NS::VulkanDeviceBuilder::finish(VulkanPhysicalDevice& physical_device) -> VulkanDevice {
    auto& logger = g_ENGINE->logger.get(engine::LogNamespaces::VULKAN);
    ZoneScoped;

    std::uint32_t count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device.device, &count, nullptr);
    std::vector<VkQueueFamilyProperties> properties(count);
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device.device, &count, properties.data());

    struct FamilyAllocation {
            std::uint32_t family    = 0;
            std::uint32_t count     = 0;
            std::uint32_t max_count = 0;
            std::vector<float> priorities;
    };

    tsl::robin_map<std::string, VulkanQueue> queues;
    tsl::robin_set<std::string> allocated_queues;
    std::vector<FamilyAllocation> queue_family_allocations;

    for (std::uint32_t idx = 0; idx < properties.size(); idx++) {
        queue_family_allocations.emplace_back(idx, 0, properties[idx].queueCount, std::vector<float>{});
    }

    for (auto& [name, requested] : queues_) {
        std::uint32_t queue_family_index = 0;
        std::uint32_t best_queue_family  = 0;
        std::uint32_t best_difference    = ~std::uint32_t(0);
        for (auto& queue : properties) {
            if (queue_family_allocations[queue_family_index].count >= queue.queueCount) {
                queue_family_index++;
                continue;
            }

            auto difference    = queue.queueFlags - static_cast<VkQueueFlags>(requested);
            auto enabled_flags = queue.queueFlags & static_cast<VkQueueFlags>(requested);
            if (enabled_flags == static_cast<VkQueueFlags>(requested) && difference < best_difference) {
                best_queue_family = queue_family_index;
                best_difference   = difference;
            }
            queue_family_index++;
        }

        auto queues_with_same_type = 1;
        for (auto& [_, queue] : queues) {
            if (queue.type_ == requested) {
                queues_with_same_type += 1;
            }
        }

        std::uint32_t idx = queue_family_allocations[best_queue_family].count;
        queue_family_allocations[best_queue_family].count++;
        queue_family_allocations[best_queue_family].priorities.push_back(1.f / static_cast<float>(queues_with_same_type));

        queues.insert({name, VulkanQueue(best_queue_family, std::min(idx, properties[best_queue_family].queueCount - 1),
                                         properties[best_queue_family].queueCount, requested)});
        allocated_queues.insert(name);

        if (allocated_queues.size() == queues_.size()) {
            break;
        }
    }

    std::vector<VkDeviceQueueCreateInfo> queue_create_info;
    for (auto& family : queue_family_allocations) {
        VkDeviceQueueCreateInfo info{};
        info.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        info.queueCount       = std::min(family.count, family.max_count);
        info.queueFamilyIndex = family.family;
        info.pQueuePriorities = family.priorities.data();

        if (info.queueCount == 0) {
            continue;
        }

        queue_create_info.push_back(info);
    }

    std::vector<const char*> extensions;
    for (auto& extension : physical_device.extensions) {
        extensions.push_back(extension.c_str());
        logger.debug("Loading extension '{}'", extension);
    }

    VkDeviceCreateInfo create_info{};
    create_info.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.pNext                   = &physical_device.features;

    create_info.queueCreateInfoCount    = static_cast<std::uint32_t>(queue_create_info.size());
    create_info.pQueueCreateInfos       = queue_create_info.data();

    create_info.enabledExtensionCount   = static_cast<std::uint32_t>(extensions.size());
    create_info.ppEnabledExtensionNames = extensions.data();

    return VulkanDevice(std::move(queues), physical_device, create_info);
}

auto ENGINE_NS::VulkanDeviceBuilder::request_queue(std::string name, VulkanQueueType type) -> VulkanDeviceBuilder& {
    queues_.insert({name, type});
    if (!queue_counts_.contains(type)) {
        queue_counts_.insert({type, 0});
    }
    queue_counts_.at(type)++;
    return *this;
}

ENGINE_NS::VulkanDeviceBuilder::VulkanDeviceBuilder() {
}
