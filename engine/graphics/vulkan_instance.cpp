#include "engine/engine.h"
#include "engine/graphics/vulkan.h"
#include "engine/logger.h"
// clang-format disable
#include <Volk/volk.h>
// clang-format enable
#include <SDL3/SDL_vulkan.h>
#include <Tracy/Tracy.hpp>
#include <cstdint>
#include <type_traits>
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
    std::copy(instance_extensions, instance_extensions + count, extensions.begin());

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
