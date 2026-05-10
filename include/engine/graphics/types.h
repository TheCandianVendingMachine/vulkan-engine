#pragma once
#include "engine/meta_defines.h"

#include <linalg/matrix.h>
#include <linalg/vector.h>
#include <vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>

namespace ENGINE_NS {
    struct ImageAllocation {
            VkImage image            = VK_NULL_HANDLE;
            VkImageView view         = VK_NULL_HANDLE;
            VmaAllocation allocation = VK_NULL_HANDLE;
            VkExtent3D extent{};
            VkFormat format{};

            ImageAllocation()                                              = default;
            ImageAllocation(const ImageAllocation& rhs)                    = default;
            auto operator=(const ImageAllocation& rhs) -> ImageAllocation& = default;

            ImageAllocation(ImageAllocation&& rhs) noexcept;
            auto operator=(ImageAllocation&& rhs) noexcept -> ImageAllocation&;

            ~ImageAllocation();

        private:
            bool will_be_destroyed_ = false;
            bool moved_             = false;
            friend class GraphicsMainDeletionQueue;
            friend class GraphicsPerFrameDeletionQueue;
            friend class GraphicsUploadDeletionQueue;
            friend class GraphicsRegisteredPipelineDeletionQueue;
    };

    struct BufferAllocation {
            VkBuffer buffer                                                  = VK_NULL_HANDLE;
            VmaAllocation allocation                                         = VK_NULL_HANDLE;
            VmaAllocationInfo info                                           = {};

            BufferAllocation()                                               = default;
            BufferAllocation(const BufferAllocation& rhs)                    = default;
            auto operator=(const BufferAllocation& rhs) -> BufferAllocation& = default;

            BufferAllocation(BufferAllocation&& rhs) noexcept;
            auto operator=(BufferAllocation&& rhs) noexcept -> BufferAllocation&;

            ~BufferAllocation();

        private:
            bool will_be_destroyed_ = false;
            bool moved_             = false;
            friend class GraphicsMainDeletionQueue;
            friend class GraphicsPerFrameDeletionQueue;
            friend class GraphicsUploadDeletionQueue;
            friend class GraphicsRegisteredPipelineDeletionQueue;
    };

    struct Vertex {
            ::linalg::Vector3<float> position{};
            float uv_x = 0.f;
            ::linalg::Vector3<float> normal{};
            float uv_y = 0.f;
            ::linalg::Vector4<float> colour{};
    };

    struct GPUMeshBuffers {
            BufferAllocation index_buffer{};
            BufferAllocation vertex_buffer{};
            VkDeviceAddress vertex_buffer_address{};
    };

    struct GPUDrawPushConstants {
            ::linalg::Matrix4<float> world_matrix = ::linalg::Matrix4<float>::identity();
            VkDeviceAddress vertex_buffer{};
    };

} // namespace ENGINE_NS
