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
            ::ENGINE_NS::crash(ErrorCode::VULKAN_ERROR, __LINE__, __func__, __FILE__);                                                     \
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
            auto with_validation_layers(bool with) -> VulkanInstanceBuilder&;

            auto finish() -> VulkanInstance;

        private:
            VulkanInstanceBuilder();
            friend class VulkanInstance;

            std::string game_name_{};
            std::string engine_name_{};
            Version game_version_{};
            Version engine_version_{};
            bool with_validation_layers_ = false;
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

            bool moved_               = false;
            bool has_debug_messenger_ = false;
            VkDebugUtilsMessengerEXT debug_messenger_;
            VkInstance instance_;
    };
} // namespace ENGINE_NS
