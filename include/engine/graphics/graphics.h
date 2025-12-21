#pragma once
#include "engine/deletion_queue.h"
#include "engine/graphics/descriptor.h"
#include "engine/graphics/pipeline.h"
#include "engine/graphics/types.h"
#include "engine/graphics/util.h"
#include "engine/graphics/vulkan.h"
#include "engine/meta_defines.h"
#include "engine/rwlock.h"
#include "linalg/vector.h"
#include <atomic>
#include <chrono>
#include <cstdint>
#include <thread>
#include <vk_mem_alloc.h>

namespace tracy {
    class VkCtx;
}

struct SDL_Window;
namespace ENGINE_NS {
    namespace graphics {
        constexpr std::size_t FRAME_OVERLAP = 2;
        struct FrameData {
                GraphicsPerFrameDeletionQueue deletion_queue{};
                VkCommandPool command_pool          = VK_NULL_HANDLE;
                VkCommandBuffer main_command_buffer = VK_NULL_HANDLE;
                VkSemaphore swapchain_semaphore_    = VK_NULL_HANDLE;
                VkFence render_fence_               = VK_NULL_HANDLE;
                tracy::VkCtx* tracy_context_        = nullptr;
        };
    } // namespace graphics

    class GraphicsEngine {
        public:
            auto initialise() -> void;
            auto draw() -> void;
            auto cleanup() -> void;

            auto current_frame() -> RwLock<graphics::FrameData>&;

            RwLock<graphics::ImGui> imgui{};

        private:
            bool initialised_ = false;

            GraphicsMainDeletionQueue deletion_queue_{};
            VmaAllocator allocator_{};

            DescriptorAllocator global_descriptor_allocator_{};
            VkDescriptorSet draw_image_descriptors_ = VK_NULL_HANDLE;
            VulkanDescriptorSetLayout draw_image_layout_{};

            ComputePipeline gradient_pipeline_{};
            GraphicsPipeline triangle_pipeline_{};

            std::thread render_thread_;
            std::chrono::milliseconds update_rate_;
            std::atomic<bool> running_;

            std::atomic<std::uint64_t> frame_number_                     = 0;
            RwLock<graphics::FrameData> frames_[graphics::FRAME_OVERLAP] = {};
            VkQueue graphics_queue_                                      = VK_NULL_HANDLE;
            VkQueue transfer_queue_                                      = VK_NULL_HANDLE;

            ::linalg::Vector2<int> window_extent_{1'700, 900};

            SDL_Window* window_ = nullptr;
            friend class Engine;

            ImageAllocation draw_image_;
            VulkanInstance vulkan_instance_;
            VulkanSurface surface_;
            VulkanPhysicalDevice physical_device_;
            VulkanDevice device_;
            VulkanSwapchain swapchain_;

            auto init_vulkan_() -> void;
            auto create_swapchain_() -> void;
            auto init_descriptors_() -> void;
            auto init_imgui_() -> void;

            auto init_pipelines_() -> void;
            auto init_background_pipelines_() -> void;
            auto init_triangle_pipeline_() -> void;

            auto draw_imgui_(VkCommandBuffer cmd, VkImageView image) -> void;
            auto draw_background_(VkCommandBuffer cmd) -> void;
            auto draw_geometry_(VkCommandBuffer cmd) -> void;
            auto draw_() -> void;
    };
} // namespace ENGINE_NS
