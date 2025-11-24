#pragma once
#include "engine/engine_utils.h"
#include "engine/meta_defines.h"
#include "engine/version.h"
#include <string>
#define VK_NO_PROTOTYPES
#include <cstdint>
#include <linalg/vector.h>
#include <robin_map.h>
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

            auto cleanup() -> void;

            VulkanInstance() = default;

        private:
            friend class VulkanInstanceBuilder;

            VulkanInstance(VkInstanceCreateInfo create_info);

            VkDebugUtilsMessengerEXT debug_messenger_ = VK_NULL_HANDLE;
            VkInstance instance_                      = VK_NULL_HANDLE;
            bool moved_                               = false;
            bool has_debug_messenger_                 = false;
    };

    class VulkanSurface {
        public:
            VulkanSurface() = default;
            VulkanSurface(SDL_Window* window, VulkanInstance& instance);

            auto cleanup() -> void;
            auto operator=(VulkanSurface&& rhs) -> VulkanSurface&;

            VkSurfaceKHR& surface = surface_;

        private:
            VkSurfaceKHR surface_     = VK_NULL_HANDLE;
            VulkanInstance* instance_ = nullptr;
            SDL_Window* window_       = nullptr;
            bool moved_               = false;
    };

    class VulkanPhysicalDevice;
    class VulkanPhysicalDeviceSelector {
        public:
            auto finish(VulkanInstance& instance) -> VulkanPhysicalDevice;

            auto set_minimum_vulkan_version(Version version) -> VulkanPhysicalDeviceSelector&;
            auto set_required_features_14(VkPhysicalDeviceVulkan14Features features) -> VulkanPhysicalDeviceSelector&;
            auto set_required_features_13(VkPhysicalDeviceVulkan13Features features) -> VulkanPhysicalDeviceSelector&;
            auto set_required_features_12(VkPhysicalDeviceVulkan12Features features) -> VulkanPhysicalDeviceSelector&;
            auto set_required_features_11(VkPhysicalDeviceVulkan11Features features) -> VulkanPhysicalDeviceSelector&;
            auto set_required_features_10(VkPhysicalDeviceFeatures features) -> VulkanPhysicalDeviceSelector&;
            auto with_extension(std::string extension) -> VulkanPhysicalDeviceSelector&;

        private:
            std::vector<std::string> extensions_;

            SDL_Window* window_ = nullptr;
            VkPhysicalDeviceVulkan14Features features_14_{};
            VkPhysicalDeviceVulkan13Features features_13_{};
            VkPhysicalDeviceVulkan12Features features_12_{};
            VkPhysicalDeviceVulkan11Features features_11_{};
            VkPhysicalDeviceFeatures features_10_{};
            Version vulkan_version_{};

            VulkanPhysicalDeviceSelector(SDL_Window* window);
            friend class VulkanPhysicalDevice;
    };

    class VulkanPhysicalDevice {
        public:
            static auto choose(SDL_Window* window) -> VulkanPhysicalDeviceSelector;

            VkPhysicalDevice& device = device_;

            auto operator=(VulkanPhysicalDevice&& rhs) noexcept -> VulkanPhysicalDevice&;

            VulkanPhysicalDevice()                     = default;
            const VkPhysicalDeviceFeatures2& features  = features_;
            const std::vector<std::string>& extensions = extensions_;


        private:
            std::vector<std::string> extensions_;

            VkPhysicalDevice device_ = VK_NULL_HANDLE;
            VkPhysicalDeviceFeatures2 features_{};
            VkPhysicalDeviceVulkan14Features features_14_{};
            VkPhysicalDeviceVulkan13Features features_13_{};
            VkPhysicalDeviceVulkan12Features features_12_{};
            VkPhysicalDeviceVulkan11Features features_11_{};
            VkPhysicalDeviceFeatures features_10_{};
            bool moved_ = false;

            VulkanPhysicalDevice(VkPhysicalDevice device, VkPhysicalDeviceFeatures f10, VkPhysicalDeviceVulkan11Features f11,
                                 VkPhysicalDeviceVulkan12Features f12, VkPhysicalDeviceVulkan13Features f13,
                                 VkPhysicalDeviceVulkan14Features f14, std::vector<std::string>&& extensions);
            friend class VulkanPhysicalDeviceSelector;
    };

    enum class VulkanQueueType : std::uint8_t {
        GRAPHICS = VK_QUEUE_GRAPHICS_BIT,
        COMPUTE  = VK_QUEUE_COMPUTE_BIT,
        TRANSFER = VK_QUEUE_TRANSFER_BIT
    };
    inline auto operator|(const VulkanQueueType& lhs, const VulkanQueueType& rhs) -> VulkanQueueType {
        return static_cast<VulkanQueueType>(static_cast<std::uint8_t>(lhs) | static_cast<std::uint8_t>(rhs));
    }
    inline auto operator|=(VulkanQueueType& lhs, const VulkanQueueType& rhs) -> VulkanQueueType& {
        lhs = lhs | rhs;
        return lhs;
    }
    inline auto operator&(const VulkanQueueType& lhs, const VulkanQueueType& rhs) -> VulkanQueueType {
        return static_cast<VulkanQueueType>(static_cast<std::uint8_t>(lhs) & static_cast<std::uint8_t>(rhs));
    }
    inline auto operator&=(VulkanQueueType& lhs, const VulkanQueueType& rhs) -> VulkanQueueType& {
        lhs = lhs & rhs;
        return lhs;
    }
    inline auto operator^(const VulkanQueueType& lhs, const VulkanQueueType& rhs) -> VulkanQueueType {
        return static_cast<VulkanQueueType>(static_cast<std::uint8_t>(lhs) ^ static_cast<std::uint8_t>(rhs));
    }
    inline auto operator^=(VulkanQueueType& lhs, const VulkanQueueType& rhs) -> VulkanQueueType& {
        lhs = lhs ^ rhs;
        return lhs;
    }

    class VulkanDevice;
    class VulkanQueue {
        public:
            auto get() -> VkQueue;
            auto get() const -> VkQueue;

        private:
            VulkanQueue(std::uint32_t queue_family, std::uint32_t queue_index, std::uint32_t max_queue_index, VulkanQueueType type);

            std::uint32_t queue_family_    = 0;
            std::uint32_t queue_index_     = 0;
            std::uint32_t max_queue_index_ = 0;
            VulkanQueueType type_{};
            VulkanDevice* device_     = nullptr;
            VkQueue associated_queue_ = VK_NULL_HANDLE;

            friend class VulkanDeviceBuilder;
            friend class VulkanDevice;
    };

    class VulkanDeviceBuilder {
        public:
            auto finish(VulkanPhysicalDevice& physical_device) -> VulkanDevice;

            auto request_queue(std::string id, VulkanQueueType type) -> VulkanDeviceBuilder&;

        private:
            tsl::robin_map<std::string, VulkanQueueType> queues_;
            tsl::robin_map<VulkanQueueType, std::uint32_t> queue_counts_;

            friend class VulkanDevice;
            VulkanDeviceBuilder();
    };

    class VulkanDevice {
        public:
            VulkanDevice() = default;

            static auto build() -> VulkanDeviceBuilder;
            auto cleanup() -> void;

            auto operator=(VulkanDevice&& rhs) -> VulkanDevice&;

            VkDevice& device                                       = device_;
            const tsl::robin_map<std::string, VulkanQueue>& queues = queues_;

        private:
            tsl::robin_map<std::string, VulkanQueue> queues_;
            VkDevice device_ = VK_NULL_HANDLE;
            bool moved_      = false;


            friend class VulkanDeviceBuilder;
            VulkanDevice(tsl::robin_map<std::string, VulkanQueue>&& queues, VulkanPhysicalDevice& physical_device,
                         VkDeviceCreateInfo create_info);
    };

    class VulkanSwapchain;
    class VulkanSwapchainBuilder {
        public:
            auto finish(VulkanPhysicalDevice& physical_device, VulkanSurface& surface, VulkanDevice& device) -> VulkanSwapchain;

            auto set_desired_format(VkSurfaceFormatKHR format) -> VulkanSwapchainBuilder&;
            auto set_present_mode(VkPresentModeKHR present) -> VulkanSwapchainBuilder&;
            auto set_extent(VkExtent2D extent) -> VulkanSwapchainBuilder&;
            auto set_extent(::linalg::Vector2<unsigned int> extent) -> VulkanSwapchainBuilder&;
            auto add_image_usage_flags(VkImageUsageFlags flags) -> VulkanSwapchainBuilder&;


        private:
            VkSurfaceFormatKHR desired_format_{};
            VkPresentModeKHR present_mode_{};
            VkExtent2D extent_{};
            VkImageUsageFlags image_usage_flags_{};

            friend class VulkanSwapchain;
            VulkanSwapchainBuilder() = default;
    };

    class VulkanSwapchain {
        public:
            VulkanSwapchain() = default;

            static auto build() -> VulkanSwapchainBuilder;
            auto operator=(VulkanSwapchain&& rhs) -> VulkanSwapchain&;
            auto cleanup() -> void;

        private:
            VulkanDevice* device_     = nullptr;
            VkSwapchainKHR swapchain_ = VK_NULL_HANDLE;
            VkFormat format_{};
            VkExtent2D extent_{};
            std::vector<VkImage> images_;
            std::vector<VkImageView> views_;
            bool moved_ = false;

            friend class VulkanSwapchainBuilder;
            VulkanSwapchain(VkSwapchainCreateInfoKHR create_info, VulkanDevice& device);
    };
} // namespace ENGINE_NS
