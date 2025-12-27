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

            ~ImageAllocation();

        private:
            bool will_be_destroyed_ = false;
            friend class GraphicsMainDeletionQueue;
            friend class GraphicsPerFrameDeletionQueue;
    };

    struct BufferAllocation {
            VkBuffer buffer          = VK_NULL_HANDLE;
            VmaAllocation allocation = VK_NULL_HANDLE;
            VmaAllocationInfo info   = {};

            ~BufferAllocation();

        private:
            bool will_be_destroyed_ = false;
            friend class GraphicsMainDeletionQueue;
            friend class GraphicsPerFrameDeletionQueue;
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
