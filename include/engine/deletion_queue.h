#pragma once
#include "engine/meta_defines.h"
#define VK_NO_PROTOTYPES
#include <cstdint>
#include <type_traits>
#include <vector>
#include <vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>

namespace ENGINE_NS {
    template <typename T>
    struct Deletion {
            T object;
            std::uint64_t idx = 0;

            template <typename = std::enable_if<std::is_copy_constructible<T>::value>::type>
            Deletion(T object, std::uint64_t idx) : object(object), idx(idx) {
            }
            template <typename = std::enable_if<std::is_move_constructible<T>::value>::type>
            Deletion(T&& object, std::uint64_t idx) : object(std::move(object)), idx(idx) {
            }

            template <typename = std::enable_if<std::is_copy_constructible<T>::value>::type>
            auto operator=(const Deletion& rhs) -> Deletion& {
                if (&rhs != this) {
                    object = rhs.object;
                    idx    = rhs.idx;
                }
                return *this;
            }
            template <typename = std::enable_if<std::is_move_assignable<T>::value>::type>
            auto operator=(const Deletion&& rhs) -> Deletion& {
                if (&rhs != this) {
                    object = std::move(rhs.object);
                    idx    = std::move(rhs.idx);
                }
                return *this;
            }

            friend auto operator<(const Deletion& lhs, const Deletion& rhs) -> bool {
                return lhs.idx < rhs.idx;
            }
            friend auto operator>(const Deletion& lhs, const Deletion& rhs) -> bool {
                return lhs.idx > rhs.idx;
            }
    };

    class IDeletionQueue {
        public:
            virtual auto flush() -> void = 0;
    };
    class GraphicsPerFrameDeletionQueue : public IDeletionQueue {
        public:
            virtual auto flush() -> void override final;

        private:
            std::uint64_t index_ = 0;
    };
} // namespace ENGINE_NS
