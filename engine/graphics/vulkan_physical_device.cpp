#include "engine/engine.h"
#include "engine/graphics/vulkan.h"
#include "engine/logger.h"
// clang-format disable
#include <Volk/volk.h>
// clang-format enable
#include <Tracy/Tracy.hpp>
#include <algorithm>
#include <cstdint>
#include <type_traits>
#include <utility>
#include <vector>

auto ENGINE_NS::VulkanPhysicalDevice::choose(SDL_Window* window) -> VulkanPhysicalDeviceSelector {
    return VulkanPhysicalDeviceSelector(window);
}

auto ENGINE_NS::VulkanPhysicalDevice::operator=(VulkanPhysicalDevice&& rhs) noexcept -> VulkanPhysicalDevice& {
    if (this != &rhs && !rhs.moved_) {
        extensions_        = std::move(rhs.extensions_);

        device_            = std::move(rhs.device_);

        features_          = std::move(rhs.features_);
        features_10_       = std::move(rhs.features_10_);
        features_11_       = std::move(rhs.features_11_);
        features_12_       = std::move(rhs.features_12_);
        features_13_       = std::move(rhs.features_13_);
        features_14_       = std::move(rhs.features_14_);

        features_.pNext    = &features_11_;
        features_11_.pNext = &features_12_;
        features_12_.pNext = &features_13_;
        features_13_.pNext = &features_14_;

        rhs.moved_         = true;
    }
    return *this;
}

ENGINE_NS::VulkanPhysicalDevice::VulkanPhysicalDevice(VkPhysicalDevice device, VkPhysicalDeviceFeatures f10,
                                                      VkPhysicalDeviceVulkan11Features f11, VkPhysicalDeviceVulkan12Features f12,
                                                      VkPhysicalDeviceVulkan13Features f13, VkPhysicalDeviceVulkan14Features f14,
                                                      std::vector<std::string>&& extensions) :
    device_(device), features_10_(f10), features_11_(f11), features_12_(f12), features_13_(f13), features_14_(f14),
    extensions_(extensions) {
    ZoneScoped;

    features_.sType    = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features_.features = features_10_;
    features_.pNext    = &features_11_;
    features_11_.pNext = &features_12_;
    features_12_.pNext = &features_13_;
    features_13_.pNext = &features_14_;
}

auto ENGINE_NS::VulkanPhysicalDeviceSelector::finish(VulkanInstance& instance) -> VulkanPhysicalDevice {
    ZoneScoped;

    auto& logger               = g_ENGINE->logger.get(engine::LogNamespaces::VULKAN);

    std::uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(instance.instance, &device_count, nullptr);
    if (device_count == 0) {
        crash(ErrorCode::VULKAN_ERROR);
    }
    std::vector<VkPhysicalDevice> devices(device_count);
    vkEnumeratePhysicalDevices(instance.instance, &device_count, devices.data());

    const auto api_version = VK_MAKE_API_VERSION(0, vulkan_version_.major, vulkan_version_.minor, vulkan_version_.patch);

    logger.debug("Requesting a device with API version {}", api_version);

    std::vector<std::pair<VkPhysicalDevice, int>> available_devices{};

    for (auto& device : devices) {
        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(device, &properties);
        logger.debug("Found device '{}' with API version {}", properties.deviceName, properties.apiVersion);
        if (properties.apiVersion < api_version) {
            logger.debug("Invalid API version");
            continue;
        }
        VkPhysicalDeviceFeatures2 features{};
        VkPhysicalDeviceVulkan11Features features11{};
        VkPhysicalDeviceVulkan12Features features12{};
        VkPhysicalDeviceVulkan13Features features13{};
        VkPhysicalDeviceVulkan14Features features14{};

        features.sType   = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        features11.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
        features12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
        features13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
        features14.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES;


        features.pNext   = &features11;
        features11.pNext = &features12;
        features12.pNext = &features13;
        features13.pNext = &features14;

        vkGetPhysicalDeviceFeatures2(device, &features);

        bool has_all_features_ = true;
        for (auto idx = 0; idx < (&features_10_.inheritedQueries - &features_10_.robustBufferAccess); idx++) {
            auto base_target = &features_10_.robustBufferAccess;
            auto base_test   = &features.features.robustBufferAccess;

            if (*(base_target + idx) == VK_TRUE && *(base_test + idx) == VK_FALSE) {
                has_all_features_ = false;
                break;
            }
        }
        if (!has_all_features_) {
            logger.debug("Device does not have a desired Vulkan 1.0 feature");
            continue;
        }

        has_all_features_ = true;
        for (auto idx = 0; idx < (&features_11_.shaderDrawParameters - &features_11_.storageBuffer16BitAccess); idx++) {
            auto base_target = &features_11_.storageBuffer16BitAccess;
            auto base_test   = &features11.storageBuffer16BitAccess;

            if (*(base_target + idx) == VK_TRUE && *(base_test + idx) == VK_FALSE) {
                has_all_features_ = false;
                break;
            }
        }
        if (!has_all_features_) {
            logger.debug("Device does not have a desired Vulkan 1.1 feature");
            continue;
        }

        has_all_features_ = true;
        for (auto idx = 0; idx < (&features_12_.subgroupBroadcastDynamicId - &features_12_.samplerMirrorClampToEdge); idx++) {
            auto base_target = &features_12_.samplerMirrorClampToEdge;
            auto base_test   = &features12.samplerMirrorClampToEdge;

            if (*(base_target + idx) == VK_TRUE && *(base_test + idx) == VK_FALSE) {
                has_all_features_ = false;
                break;
            }
        }
        if (!has_all_features_) {
            logger.debug("Device does not have a desired Vulkan 1.2 feature");
            continue;
        }

        has_all_features_ = true;
        for (auto idx = 0; idx < (&features_13_.maintenance4 - &features_13_.robustImageAccess); idx++) {
            auto base_target = &features_13_.robustImageAccess;
            auto base_test   = &features13.robustImageAccess;

            if (*(base_target + idx) == VK_TRUE && *(base_test + idx) == VK_FALSE) {
                has_all_features_ = false;
                break;
            }
        }
        if (!has_all_features_) {
            logger.debug("Device does not have a desired Vulkan 1.3 feature");
            continue;
        }

        has_all_features_ = true;
        for (auto idx = 0; idx < (&features_14_.pushDescriptor - &features_14_.globalPriorityQuery); idx++) {
            auto base_target = &features_14_.globalPriorityQuery;
            auto base_test   = &features14.globalPriorityQuery;

            if (*(base_target + idx) == VK_TRUE && *(base_test + idx) == VK_FALSE) {
                has_all_features_ = false;
                break;
            }
        }
        if (!has_all_features_) {
            logger.debug("Device does not have a desired Vulkan 1.4 feature");
            continue;
        }

        logger.debug("Device has all requested features");

        std::uint32_t extension_count = 0;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);
        std::vector<VkExtensionProperties> device_extensions(extension_count);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, device_extensions.data());

        bool found = false;
        for (auto& desired_extension : extensions_) {
            found = false;
            for (auto& device_extension : device_extensions) {
                if (device_extension.extensionName == desired_extension) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                logger.debug("Could not find extension '{}' in device", desired_extension);
                break;
            }
        }
        if (!found) {
            logger.debug("Device does not all wanted extensions");
            continue;
        }
        logger.debug("Device has all requested extensions");

        int score = 10'000;
        if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
            score -= 1'374;
        }
        if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU) {
            score -= 874;
        }
        if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU) {
            score -= 7'374;
        }
        if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_OTHER) {
            score -= 377;
        }
        logger.debug("Device has a score of {}", score);
        available_devices.push_back(std::make_pair(device, score));
    }

    std::sort(available_devices.begin(), available_devices.end(), [](auto& lhs, auto& rhs) {
        return lhs.second > rhs.second;
    });

    if (available_devices.empty()) {
        ::ENGINE_NS::crash(ErrorCode::VULKAN_ERROR, __LINE__, __func__, __FILE__);
    }

    return VulkanPhysicalDevice(available_devices[0].first, features_10_, features_11_, features_12_, features_13_, features_14_,
                                std::move(extensions_));
}

auto ENGINE_NS::VulkanPhysicalDeviceSelector::set_minimum_vulkan_version(Version version) -> VulkanPhysicalDeviceSelector& {
    vulkan_version_ = version;
    return *this;
}

auto ENGINE_NS::VulkanPhysicalDeviceSelector::set_required_features_14(VkPhysicalDeviceVulkan14Features features)
    -> VulkanPhysicalDeviceSelector& {
    features_14_ = features;
    return *this;
}

auto ENGINE_NS::VulkanPhysicalDeviceSelector::set_required_features_13(VkPhysicalDeviceVulkan13Features features)
    -> VulkanPhysicalDeviceSelector& {
    features_13_ = features;
    return *this;
}

auto ENGINE_NS::VulkanPhysicalDeviceSelector::set_required_features_12(VkPhysicalDeviceVulkan12Features features)
    -> VulkanPhysicalDeviceSelector& {
    features_12_ = features;
    return *this;
}

auto ENGINE_NS::VulkanPhysicalDeviceSelector::set_required_features_11(VkPhysicalDeviceVulkan11Features features)
    -> VulkanPhysicalDeviceSelector& {
    features_11_ = features;
    return *this;
}

auto ENGINE_NS::VulkanPhysicalDeviceSelector::set_required_features_10(VkPhysicalDeviceFeatures features) -> VulkanPhysicalDeviceSelector& {
    features_10_ = features;
    return *this;
}

auto ENGINE_NS::VulkanPhysicalDeviceSelector::with_extension(std::string extension) -> VulkanPhysicalDeviceSelector& {
    extensions_.emplace_back(extension);
    return *this;
}

ENGINE_NS::VulkanPhysicalDeviceSelector::VulkanPhysicalDeviceSelector(SDL_Window* window) : window_(window) {
    features_11_.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
    features_12_.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    features_13_.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    features_14_.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES;
}
