#pragma once
#include "engine/ecs/defines.h"
#include "engine/ecs/query.h"
#include "engine/meta_defines.h"
#include "engine/pool.h"
#include <optional>
#include <robin_map.h>
#include <string>
#include <type_traits>

namespace ENGINE_NS {
    namespace ecs {
        struct Component {};
        class ComponentRegister {
            public:
                auto register_component_by_name(std::string_view name) -> ComponentGid;
                template <typename T, typename = std::enable_if_t<std::is_base_of<Component, T>::value>>
                auto register_component() -> ComponentGid {
                    return register_component_by_name(T::Meta::name);
                }

                auto component_gid_by_name(std::string_view name) -> std::optional<ComponentGid>;
                template <typename T, typename = std::enable_if_t<std::is_base_of<Component, T>::value>>
                auto component_gid() -> std::optional<ComponentGid> {
                    return component_gid_by_name(T::Meta::name);
                }

                auto query() -> QueryBuilder;

            private:
                ComponentGid counter_ = ComponentGid(0);
                tsl::robin_map<std::string, ComponentGid> register_;
        };

        class ComponentStoreInterface {
            public:
                virtual auto create(EntityUid assigned) -> void                                                     = 0;
                virtual auto destroy(EntityUid component) -> void                                                   = 0;
                virtual auto fetch(const std::vector<EntityUid>& components) const -> std::vector<const Component*> = 0;
                virtual auto fetch_mut(const std::vector<EntityUid>& components) -> std::vector<Component*>;

                virtual auto fetch(EntityUid entity) const -> const Component* = 0;
                virtual auto fetch_mut(EntityUid entity) -> Component*;
        };

        template <typename T>
        class ComponentStore : public ComponentStoreInterface {
            public:
                static constexpr auto name() -> const char* {
                    return T::Meta::name;
                }
                virtual auto create(EntityUid assigned) -> void {
                    assignment_.insert({assigned, components_.allocate()});
                }
                virtual auto destroy(EntityUid component) -> void {
                    if (!assignment_.contains(component)) {
                        return;
                    }
                    auto& borrow = assignment_.at(component);
                    components_.free(borrow);
                }
                virtual auto fetch(const std::vector<EntityUid>& entities) const -> std::vector<const Component*> {
                    std::vector<const Component*> components;
                    for (auto& entity : entities) {
                        auto& borrow = assignment_.at(entity);
                        components.emplace_back(borrow.get().value());
                    }
                    return components;
                }
                virtual auto fetch(EntityUid entity) const -> const Component* {
                    return assignment_.at(entity).get().value();
                }

            private:
                Pool<T> components_;
                tsl::robin_map<EntityUid, pool::Borrow<T>> assignment_;
        };
    }; // namespace ecs
} // namespace ENGINE_NS
