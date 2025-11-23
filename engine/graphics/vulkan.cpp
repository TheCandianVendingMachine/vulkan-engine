#include "engine/graphics/vulkan.h"
#include "engine/engine.h"
#include "engine/logger.h"
// clang-format disable
#include <Volk/volk.h>
// clang-format enable
#include <SDL3/SDL_vulkan.h>
#include <Tracy/Tracy.hpp>
#include <algorithm>
#include <cstdint>
#include <robin_set.h>
#include <type_traits>
#include <utility>
#include <vector>


static VKAPI_ATTR VkBool32 VKAPI_CALL validation_callback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                          VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                          const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void*) {
    ZoneScoped;
    engine::Logger* logger = nullptr;
    switch (messageType) {
        case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
            logger = &ENGINE_NS::g_ENGINE->logger.get(ENGINE_NS::LogNamespaces::VULKAN);
            break;
        case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
            logger = &ENGINE_NS::g_ENGINE->logger.get(ENGINE_NS::LogNamespaces::VULKAN_VALIDATION);
            break;
        case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
            logger = &ENGINE_NS::g_ENGINE->logger.get(ENGINE_NS::LogNamespaces::VULKAN_PERFORMANCE);
            break;
        default:
            assert(false);
            break;
    }

    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        logger->error("{}", pCallbackData->pMessage);
    } else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        logger->warning("{}", pCallbackData->pMessage);
    } else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
        logger->info("{}", pCallbackData->pMessage);
    } else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
        logger->debug("{}", pCallbackData->pMessage);
    }

    return VK_FALSE;
}

auto ENGINE_NS::VulkanInstance::build() -> VulkanInstanceBuilder {
    return VulkanInstanceBuilder();
}

auto ENGINE_NS::VulkanInstance::operator=(VulkanInstance&& rhs) noexcept -> VulkanInstance& {
    if (this != &rhs && !rhs.moved_) {
        instance_            = std::move(rhs.instance_);
        has_debug_messenger_ = std::move(rhs.has_debug_messenger_);
        debug_messenger_     = std::move(rhs.debug_messenger_);
        rhs.moved_           = true;
    }
    return *this;
}

auto ENGINE_NS::VulkanInstance::cleanup() -> void {
    auto& logger = g_ENGINE->logger.get(engine::LogNamespaces::VULKAN);
    if (this->moved_) {
        logger.info("Instance has already been moved or destroyed");
        return;
    }
    logger.info("Destroying instance");

    if (has_debug_messenger_) {
        vkDestroyDebugUtilsMessengerEXT(instance_, debug_messenger_, nullptr);
    }
    vkDestroyInstance(instance_, nullptr);
    this->moved_ = true;
}

ENGINE_NS::VulkanInstance::VulkanInstance(VkInstanceCreateInfo create_info) {
    ZoneScoped;

    VK_CHECK(vkCreateInstance(&create_info, nullptr, &instance_));
    volkLoadInstance(instance_);
    if (create_info.enabledLayerCount > 0) {
        VkDebugUtilsMessengerCreateInfoEXT debug_create{};
        debug_create.sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debug_create.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                       VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debug_create.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                   VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debug_create.pfnUserCallback = validation_callback;

        if (!vkCreateDebugUtilsMessengerEXT) {
            crash(ErrorCode::VULKAN_ERROR, __LINE__, __func__, __FILE__);
        }
        VK_CHECK(vkCreateDebugUtilsMessengerEXT(instance_, &debug_create, nullptr, &debug_messenger_));
        has_debug_messenger_ = true;
    }
}

auto ENGINE_NS::VulkanInstanceBuilder::game_name(std::string_view name) -> VulkanInstanceBuilder& {
    game_name_ = name;
    return *this;
}

auto ENGINE_NS::VulkanInstanceBuilder::engine_name(std::string_view name) -> VulkanInstanceBuilder& {
    engine_name_ = name;
    return *this;
}

auto ENGINE_NS::VulkanInstanceBuilder::game_version(Version version) -> VulkanInstanceBuilder& {
    game_version_ = version;
    return *this;
}

auto ENGINE_NS::VulkanInstanceBuilder::engine_version(Version version) -> VulkanInstanceBuilder& {
    engine_version_ = version;
    return *this;
}

auto ENGINE_NS::VulkanInstanceBuilder::with_validation_layers(bool with) -> VulkanInstanceBuilder& {
    with_validation_layers_ = with;
    return *this;
}

auto ENGINE_NS::VulkanInstanceBuilder::finish() -> VulkanInstance {
    ZoneScoped;

    VkApplicationInfo app_info{};
    app_info.apiVersion         = VK_API_VERSION_1_3;
    app_info.applicationVersion = VK_MAKE_VERSION(game_version_.major, game_version_.minor, game_version_.patch);
    app_info.engineVersion      = VK_MAKE_VERSION(engine_version_.major, engine_version_.minor, engine_version_.patch);
    app_info.pApplicationName   = game_name_.c_str();
    app_info.pEngineName        = engine_name_.c_str();
    app_info.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;

    VkInstanceCreateInfo instance_info{};
    instance_info.sType            = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_info.pApplicationInfo = &app_info;

    unsigned int count             = 0;
    auto instance_extensions       = SDL_Vulkan_GetInstanceExtensions(&count);

    std::vector<const char*> extensions(count);
    std::copy(instance_extensions + 0, instance_extensions + count, extensions.begin());

    std::vector<const char*> validation_layers;
    if (with_validation_layers_) {
        extensions.insert(extensions.begin(), VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        validation_layers.emplace_back("VK_LAYER_KHRONOS_validation");

        std::uint32_t layer_count = 0;
        vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
        std::vector<VkLayerProperties> available_layers(layer_count);
        vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

        for (const auto& layer : validation_layers) {
            bool found = false;
            for (const auto& existing_layer : available_layers) {
                if (std::strcmp(layer, existing_layer.layerName) == 0) {
                    found = true;
                    break;
                }
            }

            if (!found) {
                crash(ErrorCode::VULKAN_ERROR);
            }
        }
    }
    instance_info.enabledLayerCount       = static_cast<std::uint32_t>(validation_layers.size());
    instance_info.ppEnabledLayerNames     = validation_layers.data();

    instance_info.enabledExtensionCount   = static_cast<std::uint32_t>(extensions.size());
    instance_info.ppEnabledExtensionNames = extensions.data();

    return VulkanInstance(instance_info);
}

ENGINE_NS::VulkanInstanceBuilder::VulkanInstanceBuilder() {
}

auto ENGINE_NS::VulkanPhysicalDevice::choose(SDL_Window* window) -> VulkanPhysicalDeviceSelector {
    return VulkanPhysicalDeviceSelector(window);
}

auto ENGINE_NS::VulkanPhysicalDevice::operator=(VulkanPhysicalDevice&& rhs) noexcept -> VulkanPhysicalDevice& {
    if (this != &rhs && !rhs.moved_) {
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
                                                      VkPhysicalDeviceVulkan13Features f13, VkPhysicalDeviceVulkan14Features f14) :
    device_(device), features_10_(f10), features_11_(f11), features_12_(f12), features_13_(f13), features_14_(f14) {
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

    return VulkanPhysicalDevice(available_devices[0].first, features_10_, features_11_, features_12_, features_13_, features_14_);
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

ENGINE_NS::VulkanPhysicalDeviceSelector::VulkanPhysicalDeviceSelector(SDL_Window* window) : window_(window) {
    features_11_.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
    features_12_.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    features_13_.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    features_14_.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES;
}

ENGINE_NS::VulkanSurface::VulkanSurface(SDL_Window* window, VulkanInstance& instance) : window_(window), instance_(&instance) {
    if (!SDL_Vulkan_CreateSurface(window_, instance_->instance, nullptr, &surface_)) {
        ::ENGINE_NS::crash(ErrorCode::VULKAN_ERROR, __LINE__, __func__, __FILE__);
    }
}

auto ENGINE_NS::VulkanSurface::cleanup() -> void {
    auto& logger = g_ENGINE->logger.get(engine::LogNamespaces::VULKAN);
    if (moved_ || !window_) {
        logger.info("Surface has already been moved or cleaned up");
        return;
    }
    logger.info("Destroying surface");
    SDL_Vulkan_DestroySurface(instance_->instance, surface_, nullptr);
    window_   = nullptr;
    instance_ = nullptr;
    surface_  = VK_NULL_HANDLE;
}

auto ENGINE_NS::VulkanSurface::operator=(VulkanSurface&& rhs) -> VulkanSurface& {
    if (&rhs != this && !rhs.moved_) {
        this->instance_ = std::move(rhs.instance_);
        this->surface_  = std::move(rhs.surface_);
        this->window_   = std::move(rhs.window_);
        rhs.moved_      = true;
    }
    return *this;
}

auto ENGINE_NS::VulkanDevice::build() -> VulkanDeviceBuilder {
    return VulkanDeviceBuilder();
}

auto ENGINE_NS::VulkanDevice::cleanup() -> void {
    auto& logger = g_ENGINE->logger.get(engine::LogNamespaces::VULKAN);
    if (this->moved_) {
        logger.info("Device has already been moved or destroyed");
        return;
    }
    logger.info("Destroying device");
    vkDestroyDevice(device_, nullptr);
    this->device_ = VK_NULL_HANDLE;
    this->moved_  = true;
}

auto ENGINE_NS::VulkanDevice::operator=(VulkanDevice&& rhs) -> VulkanDevice& {
    if (!rhs.moved_ && &rhs != this) {
        this->device_ = std::move(rhs.device_);
        rhs.moved_    = true;
    }
    return *this;
}

ENGINE_NS::VulkanDevice::VulkanDevice(tsl::robin_map<std::string, VulkanQueue>&& queues, VulkanPhysicalDevice& physical_device,
                                      VkDeviceCreateInfo create_info) : queues_(queues) {
    ZoneScoped;

    VK_CHECK(vkCreateDevice(physical_device.device, &create_info, nullptr, &device_));
}

auto ENGINE_NS::VulkanDeviceBuilder::finish(VulkanPhysicalDevice& physical_device) -> VulkanDevice {
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
    tsl::robin_map<std::uint32_t, std::uint32_t> queue_family_map;
    tsl::robin_set<std::string> allocated_queues;
    std::vector<FamilyAllocation> queue_family_allocations;

    for (std::uint32_t idx = 0; idx < properties.size(); idx++) {
        queue_family_allocations.emplace_back(idx++, 0, properties[idx].queueCount, std::vector<float>{});
    }

    for (auto& [name, requested] : queues_) {
        std::uint32_t queue_family_index = 0;
        std::uint32_t best_queue_family  = 0;
        std::uint32_t best_difference    = ~std::uint32_t(0);
        for (auto& queue : properties) {
            if (!queue_family_map.contains(queue_family_index)) {
                queue_family_map.insert({queue_family_index, 0});
            }
            if (queue_family_map.at(queue_family_index) >= queue.queueCount) {
                queue_family_index++;
                continue;
            }

            auto difference = queue.queueFlags - static_cast<VkQueueFlags>(requested);
            if ((queue.queueFlags & static_cast<VkQueueFlags>(requested)) != 0 && difference < best_difference) {
                best_queue_family = queue_family_index;
                best_difference   = difference;
            }
            queue_family_index++;
        }

        queue_family_allocations[best_queue_family].count++;
        queue_family_allocations[best_queue_family].priorities.push_back(
            1.f / static_cast<float>(queue_family_allocations[best_queue_family].count));

        std::uint32_t& idx = queue_family_map.at(best_queue_family);
        queues.insert({
          name, VulkanQueue{.queue_family    = best_queue_family,
                            .queue_index     = idx,
                            .max_queue_index = properties[best_queue_family].queueCount,
                            .type            = requested}
        });
        allocated_queues.insert(name);
        idx += 1;

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

    VkDeviceCreateInfo create_info{};
    create_info.sType                = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.pNext                = &physical_device.features;

    create_info.queueCreateInfoCount = static_cast<std::uint32_t>(queue_create_info.size());
    create_info.pQueueCreateInfos    = queue_create_info.data();

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
