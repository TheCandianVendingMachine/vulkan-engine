#pragma once
#include "engine/deletion_queue.h"
#include "engine/graphics/descriptor.h"
#include "engine/graphics/pipeline.h"
#include "engine/graphics/types.h"
#include "engine/graphics/util.h"
#include "engine/graphics/vulkan.h"
#include "engine/meta_defines.h"
#include "engine/rwlock.h"

#include <Volk/volk.h>
#include <linalg/vector.h>
#include <robin_map.h>
#include <vk_mem_alloc.h>

#include <atomic>
#include <chrono>
#include <cstdint>
#include <future>
#include <limits>
#include <memory>
#include <mutex>
#include <optional>
#include <span>
#include <string>
#include <thread>
#include <vector>


// clang-format off
#include <Tracy/Tracy.hpp>
#include <Tracy/TracyVulkan.hpp>
// clang-format on


namespace tracy {
    class VkCtx;
}

struct SDL_Window;
namespace ENGINE_NS {
    class GraphicsEngine;

    namespace graphics {
        constexpr std::size_t FRAME_OVERLAP  = 2;
        constexpr const char* IMMEDIATE_NAME = "immediate";
        struct FrameData {
                // Allocations that are created in the process of rendering. Should never be pushed to outside the render thread
                GraphicsPerFrameDeletionQueue deletion_queue{};
                std::vector<std::uint64_t> in_use_pipelines;
                DescriptorAllocatorGrowable descriptor_allocator;
                VkCommandPool command_pool          = VK_NULL_HANDLE;
                VkCommandBuffer main_command_buffer = VK_NULL_HANDLE;
                VkSemaphore swapchain_semaphore_    = VK_NULL_HANDLE;
                VkFence render_fence_               = VK_NULL_HANDLE;
                tracy::VkCtx* tracy_context_        = nullptr;
        };

        struct MeshUpload {
                std::promise<GPUMeshBuffers> promise;
                std::vector<std::uint32_t> indices;
                std::vector<Vertex> vertices;
        };

        struct TextureUpload {
                std::promise<ImageAllocation> promise;
                VkExtent3D size{};
                VkFormat format{};
                VkImageUsageFlags usage{};
                bool mipmapped     = false;
                void* texture_data = nullptr;
        };

        class RegisteredPipeline {
            public:
                RegisteredPipeline() = default;
                RegisteredPipeline(RegisteredPipeline&& other) noexcept;
                virtual ~RegisteredPipeline();

                virtual auto operator=(RegisteredPipeline&& rhs) noexcept -> RegisteredPipeline&;

                virtual auto name() const -> std::string = 0;
                virtual auto build_graphics_pipeline(ENGINE_NS::GraphicsEngine& engine,
                                                     VulkanDevice& device,
                                                     GraphicsRegisteredPipelineDeletionQueue& deletion_queue)
                    -> std::optional<GraphicsPipelineBuilder>;
                virtual auto build_compute_pipeline(ENGINE_NS::GraphicsEngine& engine,
                                                    VulkanDevice& device,
                                                    GraphicsRegisteredPipelineDeletionQueue& deletion_queue)
                    -> std::optional<ComputePipelineBuilder>;

                auto init_pipeline(ENGINE_NS::GraphicsEngine& engine, VulkanDevice& device) -> void;
                auto destroy(VulkanDevice& device, VmaAllocator allocator) -> void;

                virtual auto record_graphics(VkCommandBuffer buffer) -> void;
                virtual auto record_compute(VkCommandBuffer buffer) -> void;
                virtual auto push_constants() -> GPUPushConstants;
                virtual auto dependencies() -> std::vector<std::string>;

                bool enabled = true;

                friend class ENGINE_NS::GraphicsEngine;

            protected:
                DescriptorAllocatorGrowable pipeline_descriptor_allocator_;
                GraphicsRegisteredPipelineDeletionQueue deletion_queue_{};
                std::optional<GraphicsPipeline> graphics_pipeline_ = std::nullopt;
                std::optional<ComputePipeline> compute_pipeline_   = std::nullopt;
                std::uint64_t id_                                  = std::numeric_limits<std::uint64_t>::max();
                std::atomic<std::uint64_t> paused_                 = 0;
                virtual auto destroy_(VulkanDevice& device, VmaAllocator allocator) -> void;

            private:
                bool moved_ = false;
        };

        class RegisteredPipelineReceipt {
            public:
                RegisteredPipelineReceipt(RegisteredPipelineReceipt&& other) noexcept;
                ~RegisteredPipelineReceipt();

                auto operator=(RegisteredPipelineReceipt&& rhs) noexcept -> RegisteredPipelineReceipt&;

            private:
                RegisteredPipelineReceipt(GraphicsEngine& engine, std::vector<std::uint64_t>&& ids);

                friend class ENGINE_NS::GraphicsEngine;
                GraphicsEngine& engine_;
                std::vector<std::uint64_t> pipeline_ids_;
                bool moved_ = false;
        };

        struct StagingBuffer {
                BufferAllocation allocation{};
                void* mapped_data      = nullptr;
                std::size_t total_size = 0;
        };

        enum class Thread : std::uint8_t {
            MAIN,
            UPLOAD,
            DRAW,
            COMPILE
        };
        auto thread_name(Thread thread) -> std::string;
        auto thread_immediate_name(Thread thread) -> std::string;
    } // namespace graphics

    class GraphicsEngine {
        public:
            auto initialise() -> void;
            auto draw() -> void;
            auto cleanup() -> void;

            auto current_frame() -> RwLock<graphics::FrameData>&;
            auto next_frame() -> RwLock<graphics::FrameData>&;

            auto allocate_buffer(std::size_t size, VkBufferUsageFlags flags, VmaMemoryUsage usage) -> BufferAllocation;
            auto destroy_buffer(BufferAllocation allocation) -> void;

            auto allocate_image(VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped = false) -> ImageAllocation;
            auto destroy_image(ImageAllocation allocation) -> void;

            auto destroy_shader(ENGINE_NS::asset::CompiledShader shader) -> void;

            auto upload_mesh(std::span<std::uint32_t> indices, std::span<Vertex> vertices) -> std::future<GPUMeshBuffers>;
            auto upload_image(void* data, VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped = false)
                -> std::future<ImageAllocation>;

            template <typename TFunc>
            auto immediate_submit(const TFunc&& func) -> void {
                ZoneScoped;
                auto thread     = thread_ids_.at(std::this_thread::get_id());
                auto& immediate = immediates_.at(thread);
                immediate.setup(device_.device);
                {
                    TracyVkZone(immediate.tracy_context, immediate.command_buffer, StaticNames::ImmediateCommandBufferName);
                    func(immediate.command_buffer);
                }
                immediate.teardown(device_.device);
            }

            auto pause_registered_pipelines() -> void;
            auto resume_registered_pipelines() -> void;

            [[nodiscard("When return value has its deconstructor called it will queue destruction of all pipelines.")]]
            auto register_pipelines(std::vector<std::unique_ptr<graphics::RegisteredPipeline>>&& pipelines)
                -> graphics::RegisteredPipelineReceipt;
            auto deregister_pipelines(std::vector<std::uint64_t>& ids) -> void;

            const RwLock<graphics::ImGui> imgui;

            ImageAllocation& draw_image = draw_image_;

        private:
            bool initialised_ = false;

            GraphicsMainDeletionQueue deletion_queue_{};
            GraphicsPerFrameDeletionQueue frame_deletion_queue_{};
            VmaAllocator allocator_{};

            DescriptorAllocatorGrowable global_descriptor_allocator_;
            VkDescriptorSet draw_image_descriptors_ = VK_NULL_HANDLE;
            VulkanDescriptorSetLayout draw_image_layout_{};

            RwLock<std::vector<std::unique_ptr<graphics::RegisteredPipeline>>> new_pipelines_;
            RwLock<tsl::robin_map<std::uint64_t, std::unique_ptr<graphics::RegisteredPipeline>>> registered_pipelines_;
            RwLock<tsl::robin_map<std::uint64_t, std::uint64_t>> in_use_pipelines_;
            RwLock<std::vector<std::unique_ptr<graphics::RegisteredPipeline>>> to_delete_pipelines_;
            std::uint64_t next_pipeline_uid_ = 0;

            std::thread render_thread_;
            std::chrono::milliseconds update_rate_;

            GraphicsUploadDeletionQueue upload_deletion_queue_{};
            RwLock<std::deque<graphics::MeshUpload>> mesh_uploads_;
            RwLock<std::deque<graphics::TextureUpload>> texture_uploads_;
            std::thread upload_thread_;
            std::atomic<bool> upload_ready_;

            std::thread pipeline_compile_thread_;
            std::mutex pipeline_compile_lock_;
            std::condition_variable pipeline_compile_condition_{};

            std::atomic<bool> running_;

            std::atomic<std::uint64_t> frame_number_                     = 0;
            RwLock<graphics::FrameData> frames_[graphics::FRAME_OVERLAP] = {};
            VkQueue graphics_queue_                                      = VK_NULL_HANDLE;
            VkQueue transfer_queue_                                      = VK_NULL_HANDLE;

            std::mutex thread_init_mutex_{};
            tsl::robin_map<graphics::Thread, graphics::Immediate> immediates_;
            tsl::robin_map<std::thread::id, graphics::Thread> thread_ids_;

            ::linalg::Vector2<int> window_extent_{1'700, 900};

            SDL_Window* window_ = nullptr;
            friend class Engine;

            ImageAllocation draw_image_;
            VulkanInstance vulkan_instance_;
            VulkanSurface surface_;
            VulkanPhysicalDevice physical_device_;
            VulkanDevice device_;
            VulkanSwapchain swapchain_;

            auto init_thread_(graphics::Thread thread) -> void;

            auto init_vulkan_() -> void;
            auto create_swapchain_() -> void;
            auto init_descriptors_() -> void;
            auto init_imgui_() -> void;
            auto init_immediates_() -> void;

            auto init_pipelines_() -> void;

            auto draw_imgui_(VkCommandBuffer cmd, VkImageView image) -> void;
            auto draw_background_(VkCommandBuffer cmd) -> void;
            auto draw_registered_(RwDataMut<graphics::FrameData>& frame, VkCommandBuffer cmd) -> void;
            auto draw_() -> void;

            auto upload_() -> void;
            auto upload_meshes_(std::vector<graphics::StagingBuffer>& staging_buffers) -> void;
            auto upload_textures_(std::vector<graphics::StagingBuffer>& staging_buffers) -> void;


            auto compile_() -> void;
    };
} // namespace ENGINE_NS
