#pragma once
#include "engine/meta_defines.h"
#define VK_NO_PROTOTYPES
#include "engine/graphics/types.h"
#include "engine/graphics/vulkan.h"
#include <cstdint>
#include <type_traits>
#include <vector>
#include <vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>

namespace ENGINE_NS {
    template <typename T>
    struct DeletionInterface {
            T object;
            std::uint64_t idx = 0;

            template <typename = std::enable_if<std::is_default_constructible<T>::value>::type>
            DeletionInterface() : object(T{}) {
            }
            template <typename = std::enable_if<std::is_copy_constructible<T>::value>::type>
            DeletionInterface(const DeletionInterface& rhs) : object(rhs.object), idx(rhs.idx) {
            }
            template <typename = std::enable_if<std::is_move_constructible<T>::value>::type>
            DeletionInterface(DeletionInterface&& rhs) : object(std::move(rhs.object)), idx(std::move(rhs.idx)) {
            }
            template <typename = std::enable_if<std::is_copy_constructible<T>::value>::type>
            DeletionInterface(T& object, std::uint64_t idx) : object(object), idx(idx) {
            }
            template <typename = std::enable_if<std::is_move_constructible<T>::value>::type>
            DeletionInterface(T&& object, std::uint64_t idx) : object(std::move(object)), idx(idx) {
            }

            template <typename = std::enable_if<std::is_copy_constructible<T>::value>::type>
            auto operator=(const DeletionInterface& rhs) -> DeletionInterface& {
                if (&rhs != this) {
                    object = rhs.object;
                    idx    = rhs.idx;
                }
                return *this;
            }
            template <typename = std::enable_if<std::is_move_assignable<T>::value>::type>
            auto operator=(DeletionInterface&& rhs) -> DeletionInterface& {
                if (&rhs != this) {
                    object = std::move(rhs.object);
                    idx    = std::move(rhs.idx);
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

    class GraphicsMainDeletionQueue {
        public:
            auto flush(VulkanDevice& device, VmaAllocator allocator) -> void;

            auto push(ImageAllocation allocation) -> void;

        private:
            Deletion<ImageAllocation> draw_image_{};
    };
    class GraphicsPerFrameDeletionQueue {
        public:
            auto flush(VulkanDevice& device, VmaAllocator allocator) -> void;

        private:
            std::uint64_t index_ = 0;
    };
} // namespace ENGINE_NS
