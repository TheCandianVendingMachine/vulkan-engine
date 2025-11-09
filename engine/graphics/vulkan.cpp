#include "engine/graphics/vulkan.h"
// clang-format disable
#include <Volk/volk.h>
// clang-format enable
#include <SDL3/SDL_vulkan.h>
#include <algorithm>
#include <type_traits>
#include <vector>

auto ENGINE_NS::VulkanInstance::build() -> VulkanInstanceBuilder {
    return VulkanInstanceBuilder();
}

auto ENGINE_NS::VulkanInstance::operator=(VulkanInstance&& rhs) noexcept -> VulkanInstance& {
    if (this != &rhs && !rhs.moved_) {
        instance_  = std::move(rhs.instance_);
        rhs.moved_ = true;
    }
    return *this;
}

ENGINE_NS::VulkanInstance::~VulkanInstance() {
    if (this->moved_) {
        return;
    }
    vkDestroyInstance(instance_, nullptr);
}

ENGINE_NS::VulkanInstance::VulkanInstance(VkInstanceCreateInfo create_info) {
    VK_CHECK(vkCreateInstance(&create_info, nullptr, &instance_));
    volkLoadInstance(instance_);
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

auto ENGINE_NS::VulkanInstanceBuilder::finish() -> VulkanInstance {
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

    std::vector<const char*> extensions(count + 1);
    extensions[0] = VK_EXT_DEBUG_REPORT_EXTENSION_NAME;
    std::copy(instance_extensions + 0, instance_extensions + count, extensions.begin() + 1);

    instance_info.enabledExtensionCount   = count;
    instance_info.ppEnabledExtensionNames = extensions.data();

    return VulkanInstance(instance_info);
}

ENGINE_NS::VulkanInstanceBuilder::VulkanInstanceBuilder() {
}

