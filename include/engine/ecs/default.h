#pragma once
#include "engine/ecs/component.h"
#include "engine/ecs/defines.h"
#include "engine/meta_defines.h"
#include "engine/reflection/type.h"

#include <fmt/format.h>

#include <string>
#include <string_view>
#include <utility>

namespace ENGINE_NS::reflection {
    template <>
    struct Type<ENGINE_NS::ecs::ComponentId> : Atom<ENGINE_NS::ecs::ComponentId> {
            using Inner = ENGINE_NS::ecs::ComponentId;

            static constexpr auto name() -> std::string_view {
                return "ComponentId";
            }
            static auto as_string(const Inner& var) -> std::string {
                return fmt::format("{}", static_cast<std::size_t>(var));
            }
            static auto as_human_string(const Inner& var) -> std::string {
                return fmt::format("{}_component_id", static_cast<std::size_t>(var));
            }
            static auto construct() -> Inner {
                return Inner{};
            }
            static auto construct(std::size_t value) -> Inner {
                return Inner{std::move(value)};
            }
            static auto cast(const Inner& arg) -> Inner {
                return arg;
            }
            static auto cast_from_ptr(void* arg) -> Inner& {
                return *reinterpret_cast<Inner*>(arg);
            }
            static auto cast_from_ptr(const void* arg) -> const Inner& {
                return *reinterpret_cast<const Inner*>(arg);
            }
    };
} // namespace ENGINE_NS::reflection

namespace ENGINE_NS {
    namespace ecs {
        /// Built-in component types supplied by the engine ECS.
        namespace predefined {
            /// Component that assigns a unique ComponentId when default-constructed.
            ///
            /// Add this component to an entity query when game code needs a stable
            /// per-entity id component in addition to the EntityUid handle.
            struct UidComponent : Component {
                    ComponentId id_;
                    UidComponent() {
                        static ComponentId next_id_;
                        id_ = ++next_id_;
                    }
                    REFLECT_START(UidComponent)
                    REFLECT_MEMBER(id_)
                    REFLECT_END;
            };
        } // namespace predefined
    } // namespace ecs
} // namespace ENGINE_NS
