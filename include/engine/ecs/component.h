#pragma once
#include "engine/ecs/defines.h"
#include "engine/ecs/query.h"
#include "engine/meta_defines.h"
#include "engine/pool.h"
#include <algorithm>
#include <memory>
#include <optional>
#include <robin_map.h>
#include <string>
#include <type_traits>

namespace ENGINE_NS {
    namespace ecs {
        struct Component {};
        class ComponentRegister {
            public:
                ComponentRegister();

                auto register_component_by_name(std::string_view name) -> ComponentGid;
                template <typename T, typename = std::enable_if_t<std::is_base_of<Component, T>::value>>
                auto register_component() -> ComponentGid {
                    return register_component_by_name(T::Meta::name);
                }

                auto component_gid_by_name(std::string_view name) const -> std::optional<ComponentGid>;
                template <typename T, typename = std::enable_if_t<std::is_base_of<Component, T>::value>>
                auto component_gid() const -> std::optional<ComponentGid> {
                    return component_gid_by_name(T::Meta::name);
                }

                auto query() const -> QueryBuilder;

            private:
                ComponentGid counter_ = ComponentGid(0);
                tsl::robin_map<std::string, ComponentGid> register_;
        };

        class Bundle {
            public:
                template <typename T, typename = std::enable_if_t<std::is_base_of<Component, T>::value>>
                auto component() -> T& {
                    auto gid       = component_map_.at(T::Meta::name);
                    auto component = stored_.at(gid);
                    return *static_cast<T*>(component);
                }
                auto component(std::string_view name) -> Component* {
                    auto gid       = component_map_.at(std::string(name));
                    auto component = stored_.at(gid);
                    return component;
                }

                const EntityUid& entity = entity_;

                Bundle(Bundle&& rhs);

            private:
                Bundle(EntityUid entity, Query query);

                template <typename T, typename = std::enable_if_t<std::is_base_of<Component, T>::value>>
                auto assign(ComponentGid gid, T* component) -> void {
                    if (query_.query.get(static_cast<std::size_t>(gid)) == 0) {
                        return;
                    }
                    stored_.insert({gid, component});
                    component_map_.insert({T::Meta::name, gid});
                }

                friend class EntityStore;
                template <typename T>
                friend class ComponentStore;

                EntityUid entity_;
                Query query_;
                tsl::robin_map<std::string, ComponentGid> component_map_;
                tsl::robin_map<ComponentGid, Component*> stored_;
        };

        class ComponentStoreInterface {
            public:
                virtual auto create(EntityUid assigned) -> void                                                     = 0;
                virtual auto destroy(EntityUid component) -> void                                                   = 0;
                virtual auto fetch(const std::vector<EntityUid>& components) const -> std::vector<const Component*> = 0;
                virtual auto fetch_mut(const std::vector<EntityUid>& components) -> std::vector<Component*>;

                virtual auto fetch(EntityUid entity) const -> const Component* = 0;
                virtual auto fetch_mut(EntityUid entity) -> Component*;

                virtual auto assign_bundles(std::vector<Bundle>& bundles) -> void = 0;
        };

        template <typename T>
        class ComponentStore : public ComponentStoreInterface {
            public:
                ComponentStore(const ComponentRegister& component_register) {
                    this->gid_ = component_register.component_gid<T>().value();
                }

                static constexpr auto name() -> const char* {
                    return T::Meta::name;
                }
                virtual auto create(EntityUid assigned) -> void override final {
                    assignment_.insert({assigned, components_.allocate()});
                }
                virtual auto destroy(EntityUid component) -> void override final {
                    if (!assignment_.contains(component)) {
                        return;
                    }
                    auto& borrow = assignment_.at(component);
                    components_.free(borrow);
                }
                virtual auto fetch(const std::vector<EntityUid>& entities) const -> std::vector<const Component*> override final {
                    std::vector<const Component*> components;
                    for (auto& entity : entities) {
                        auto& borrow = assignment_.at(entity);
                        components.emplace_back(borrow.get().value());
                    }
                    return components;
                }
                virtual auto fetch(EntityUid entity) const -> const Component* override final {
                    return assignment_.at(entity).get().value();
                }

                virtual auto assign_bundles(std::vector<Bundle>& bundles) -> void override final {
                    for (auto& bundle : bundles) {
                        if (bundle.query_.query.get(static_cast<std::size_t>(this->gid_)) == 0) {
                            continue;
                        }
                        bundle.assign(gid_, static_cast<T*>(this->fetch_mut(bundle.entity_)));
                    }
                }

            private:
                ComponentGid gid_;

                Pool<T> components_;
                tsl::robin_map<EntityUid, pool::Borrow<T>> assignment_;
        };
    }; // namespace ecs
} // namespace ENGINE_NS
