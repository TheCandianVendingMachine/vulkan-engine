#pragma once
#include "engine/assets/library.h"
#include "engine/graphics/descriptor.h"
#include "engine/graphics/pipeline.h"
#include "engine/graphics/types.h"
#include "engine/graphics/util.h"
#include "engine/graphics/vulkan.h"
#include "engine/meta_defines.h"

#include <vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <type_traits>
#include <vector>


namespace ENGINE_NS {
    template <typename T>
    struct DeletionInterface {
            T object;
            std::uint64_t idx = 0;


            DeletionInterface()
                requires std::is_default_constructible_v<T>
                : object(T{}) {
            }

            DeletionInterface(const DeletionInterface& rhs)
                requires std::is_copy_constructible_v<T>
                : object(rhs.object), idx(rhs.idx) {
            }

            DeletionInterface(DeletionInterface&& rhs) noexcept
                requires std::is_move_constructible_v<T>
                : object(std::move(rhs.object)), idx(rhs.idx) {
            }

            DeletionInterface(T& object, std::uint64_t idx)
                requires std::is_copy_constructible_v<T>
                : object(object), idx(idx) {
            }

            DeletionInterface(T&& object, std::uint64_t idx)
                requires std::is_move_constructible_v<T>
                : object(std::move(object)), idx(idx) {
            }


            auto operator=(const DeletionInterface& rhs) -> DeletionInterface&
                requires(std::is_copy_constructible_v<T>)
            {
                if (&rhs != this) {
                    object = rhs.object;
                    idx    = rhs.idx;
                }
                return *this;
            }

            auto operator=(DeletionInterface&& rhs) noexcept -> DeletionInterface&
                requires std::is_move_assignable<T>::value
            {
                if (&rhs != this) {
                    object = std::move(rhs.object);
                    idx    = rhs.idx;
                }
                return *this;
            }

            friend auto operator<(const DeletionInterface& lhs, const DeletionInterface& rhs) -> bool {
                return lhs.idx < rhs.idx;
            }
            friend auto operator>(const DeletionInterface& lhs, const DeletionInterface& rhs) -> bool {
                return lhs.idx > rhs.idx;
            }
    };

    template <typename T>
    struct Deletion : public DeletionInterface<T> {};
    template <>
    struct Deletion<ImageAllocation> : public DeletionInterface<ImageAllocation> {
            using DeletionInterface<ImageAllocation>::DeletionInterface;
            using DeletionInterface<ImageAllocation>::operator=;
            auto destroy(VkDevice device, VmaAllocator allocator) -> void;
    };
    template <>
    struct Deletion<BufferAllocation> : public DeletionInterface<BufferAllocation> {
            using DeletionInterface<BufferAllocation>::DeletionInterface;
            using DeletionInterface<BufferAllocation>::operator=;
            auto destroy(VkDevice device, VmaAllocator allocator) -> void;
    };
    template <>
    struct Deletion<GPUMeshBuffers> : public DeletionInterface<GPUMeshBuffers> {
            using DeletionInterface<GPUMeshBuffers>::DeletionInterface;
            using DeletionInterface<GPUMeshBuffers>::operator=;
            auto destroy(VkDevice device, VmaAllocator allocator) -> void;
    };
    template <>
    struct Deletion<VulkanDescriptorSetLayout> : public DeletionInterface<VulkanDescriptorSetLayout> {
            using DeletionInterface<VulkanDescriptorSetLayout>::DeletionInterface;
            using DeletionInterface<VulkanDescriptorSetLayout>::operator=;
            auto destroy(VkDevice device) -> void;
    };
    template <>
    struct Deletion<ComputePipeline> : public DeletionInterface<ComputePipeline> {
            using DeletionInterface<ComputePipeline>::DeletionInterface;
            using DeletionInterface<ComputePipeline>::operator=;
            auto destroy(VkDevice device) -> void;
    };
    template <>
    struct Deletion<GraphicsPipeline> : public DeletionInterface<GraphicsPipeline> {
            using DeletionInterface<GraphicsPipeline>::DeletionInterface;
            using DeletionInterface<GraphicsPipeline>::operator=;
            auto destroy(VkDevice device) -> void;
    };
    template <>
    struct Deletion<asset::CompiledShader> : public DeletionInterface<asset::CompiledShader> {
            using DeletionInterface<asset::CompiledShader>::DeletionInterface;
            using DeletionInterface<asset::CompiledShader>::operator=;
            auto destroy(VkDevice device) -> void;
    };
    template <>
    struct Deletion<graphics::ImGui> : public DeletionInterface<graphics::ImGui> {
            using DeletionInterface<graphics::ImGui>::DeletionInterface;
            using DeletionInterface<graphics::ImGui>::operator=;
            auto destroy(VkDevice device) -> void;
    };
    template <>
    struct Deletion<graphics::Immediate> : public DeletionInterface<graphics::Immediate> {
            using DeletionInterface<graphics::Immediate>::DeletionInterface;
            using DeletionInterface<graphics::Immediate>::operator=;
            auto destroy(VkDevice device) -> void;
    };
    template <>
    struct Deletion<DescriptorAllocatorGrowable> : public DeletionInterface<DescriptorAllocatorGrowable> {
            using DeletionInterface<DescriptorAllocatorGrowable>::DeletionInterface;
            using DeletionInterface<DescriptorAllocatorGrowable>::operator=;
            auto destroy(VkDevice device) -> void;
    };

    class GraphicsMainDeletionQueue {
        public:
            auto flush(VulkanDevice& device, VmaAllocator allocator) -> void;

            auto push(DescriptorAllocatorGrowable& descriptor_allocator) -> void;
            auto push(ImageAllocation& allocation) -> void;
            auto push(VulkanDescriptorSetLayout layout) -> void;
            auto push(ComputePipeline pipeline) -> void;
            auto push(GraphicsPipeline pipeline) -> void;
            auto push(graphics::ImGui imgui) -> void;
            auto push(GPUMeshBuffers& buffers) -> void;
            auto push(graphics::Immediate immediate) -> void;


        private:
            std::vector<Deletion<DescriptorAllocatorGrowable>> descriptor_allocators_;
            Deletion<ImageAllocation> draw_image_{};
            Deletion<graphics::ImGui> imgui_{};
            std::vector<Deletion<ComputePipeline>> compute_pipelines_;
            std::vector<Deletion<GraphicsPipeline>> graphics_pipelines_;
            std::vector<Deletion<VulkanDescriptorSetLayout>> layouts_;
            std::vector<Deletion<GPUMeshBuffers>> mesh_buffers_;
            std::vector<Deletion<graphics::Immediate>> immediates_;
    };
    class GraphicsPerFrameDeletionQueue {
        public:
            auto flush(VulkanDevice& device, VmaAllocator allocator) -> void;

            auto push(DescriptorAllocatorGrowable& descriptor_allocator) -> void;
            auto push(ImageAllocation& allocation) -> void;
            auto push(BufferAllocation& allocation) -> void;
            auto push(VulkanDescriptorSetLayout layout) -> void;
            auto push(GPUMeshBuffers& buffers) -> void;
            auto push(asset::CompiledShader& shader) -> void;

        private:
            std::vector<Deletion<DescriptorAllocatorGrowable>> descriptor_allocators_;
            std::vector<Deletion<ImageAllocation>> images_;
            std::vector<Deletion<BufferAllocation>> buffers_;
            std::vector<Deletion<VulkanDescriptorSetLayout>> layouts_;
            std::vector<Deletion<GPUMeshBuffers>> mesh_buffers_;
            std::vector<Deletion<asset::CompiledShader>> shaders_;

            std::uint64_t index_ = 0;
    };
    class GraphicsUploadDeletionQueue {
        public:
            auto flush(VulkanDevice& device, VmaAllocator allocator) -> void;

            auto push(BufferAllocation& allocation) -> void;

        private:
            std::vector<Deletion<BufferAllocation>> buffers_;
            std::uint64_t index_ = 0;
    };
    class GraphicsRegisteredPipelineDeletionQueue {
        public:
            auto flush(VulkanDevice& device, VmaAllocator allocator) -> void;

            auto push(DescriptorAllocatorGrowable& descriptor_allocator) -> void;
            auto push(ImageAllocation& allocation) -> void;
            auto push(VulkanDescriptorSetLayout layout) -> void;
            auto push(ComputePipeline pipeline) -> void;
            auto push(GraphicsPipeline pipeline) -> void;
            auto push(GPUMeshBuffers& buffers) -> void;
            auto push(graphics::Immediate immediate) -> void;
            auto push(asset::CompiledShader& shader) -> void;

        private:
            std::vector<Deletion<DescriptorAllocatorGrowable>> descriptor_allocators_;
            std::vector<Deletion<ImageAllocation>> images_;
            std::vector<Deletion<ComputePipeline>> compute_pipelines_;
            std::vector<Deletion<GraphicsPipeline>> graphics_pipelines_;
            std::vector<Deletion<VulkanDescriptorSetLayout>> layouts_;
            std::vector<Deletion<GPUMeshBuffers>> mesh_buffers_;
            std::vector<Deletion<graphics::Immediate>> immediates_;
            std::vector<Deletion<asset::CompiledShader>> shaders_;

            std::uint64_t index_ = 0;
    };
} // namespace ENGINE_NS
