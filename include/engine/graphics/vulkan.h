#pragma once
#include "engine/engine_utils.h"
#include "engine/meta_defines.h"
#include "engine/version.h"
#include <string>
#define VK_NO_PROTOTYPES
#include <vulkan/vulkan_core.h>

#define VK_CHECK(x)                                                                                                                        \
    do {                                                                                                                                   \
        VkResult err = x;                                                                                                                  \
        if (err) {                                                                                                                         \
            ::ENGINE_NS::crash(ErrorCode::VULKAN_ERROR);                                                                                   \
        }                                                                                                                                  \
    } while (0)

struct SDL_Window;
namespace ENGINE_NS {
    class VulkanInstance;
    class VulkanInstanceBuilder {
        public:
            auto game_name(std::string_view name) -> VulkanInstanceBuilder&;
            auto engine_name(std::string_view name) -> VulkanInstanceBuilder&;
            auto game_version(Version version) -> VulkanInstanceBuilder&;
            auto engine_version(Version version) -> VulkanInstanceBuilder&;

            auto finish() -> VulkanInstance;

        private:
            VulkanInstanceBuilder();
            friend class VulkanInstance;

            std::string game_name_{};
            std::string engine_name_{};
            Version game_version_{};
            Version engine_version_{};
    };

    class VulkanInstance {
        public:
            static auto build() -> VulkanInstanceBuilder;

            VkInstance& instance = instance_;

            auto operator=(VulkanInstance&& rhs) noexcept -> VulkanInstance&;

            VulkanInstance() = default;
            ~VulkanInstance();

        private:
            friend class VulkanInstanceBuilder;

            VulkanInstance(VkInstanceCreateInfo create_info);

            bool moved_ = false;
            VkInstance instance_;
    };
} // namespace ENGINE_NS
