#pragma once
#include "engine/ecs/defines.h"
#include "engine/ecs/query.h"
#include "engine/meta_defines.h"
#include "engine/pool.h"

#include <tracy/Tracy.hpp>
#include <algorithm>
#include <memory>
#include <optional>
#include <robin_map.h>
#include <string>
#include <type_traits>
#include <vector>

namespace ENGINE_NS {
    namespace ecs {
        /// Base type for all ECS components.
        ///
        /// Components are plain data objects stored by ComponentStore<T>. Game
        /// components should derive from this type and use the reflection macros so
        /// the ECS can register them by `T::Meta::name`.
        struct Component {};

        /// Registry that assigns global ids to component types and creates queries.
        ///
        /// Register each component type once before creating entities that use it.
        /// EcsWorld owns one ComponentRegister and exposes it through
        /// `EcsWorld::component_register` for building queries in game code/systems.
        class ComponentRegister {
            public:
                ComponentRegister();

                /// Register a component type by its stable reflection/name string.
                ///
                /// Returns the ComponentGid used as the query bit and component-store
                /// key. Names are expected to be unique.
                auto register_component_by_name(std::string_view name) -> ComponentGid;

                /// Register component type T using `T::Meta::name`.
                ///
                /// T must derive from Component and provide reflection metadata.
                template <typename T, typename = std::enable_if_t<std::is_base_of<Component, T>::value>>
                auto register_component() -> ComponentGid {
                    return register_component_by_name(T::Meta::name);
                }

                /// Look up the global component type id for a registered name.
                auto component_gid_by_name(std::string_view name) const -> std::optional<ComponentGid>;

                /// Look up the global component type id for T.
                template <typename T, typename = std::enable_if_t<std::is_base_of<Component, T>::value>>
                auto component_gid() const -> std::optional<ComponentGid> {
                    return component_gid_by_name(T::Meta::name);
                }

                /// Start building a component query against this registry.
                auto query() const -> QueryBuilder;

            private:
                ComponentGid counter_ = ComponentGid(0);
                tsl::robin_map<std::string, ComponentGid> register_;
        };

        /// Mutable group of components for one entity that matched a Query.
        ///
        /// Systems receive vectors of Bundle values. Each Bundle contains pointers to
        /// the components selected by the system query for a single entity.
        class Bundle {
            public:
                /// Fetch a selected component by type.
                ///
                /// The component must have been selected by the Query used to build
                /// this Bundle, otherwise robin_map::at will throw.
                template <typename T, typename = std::enable_if_t<std::is_base_of<Component, T>::value>>
                auto component() -> T& {
                    auto gid       = component_map_.at(T::Meta::name);
                    auto component = stored_.at(gid);
                    return *static_cast<T*>(component);
                }

                /// Fetch a selected component by registered reflection/name string.
                auto component(std::string_view name) -> Component* {
                    auto gid       = component_map_.at(std::string(name));
                    auto component = stored_.at(gid);
                    return component;
                }

                /// Entity id associated with this bundle.
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

        /// Type-erased interface for component storage.
        ///
        /// EcsWorld stores these behind unique_ptr so each registered component type
        /// can have its own strongly typed ComponentStore<T>.
        class ComponentStoreInterface {
            public:
                /// Create a default component instance assigned to an entity.
                virtual auto create(EntityUid assigned) -> void                                                     = 0;
                /// Destroy the component instance assigned to an entity.
                virtual auto destroy(EntityUid component) -> void                                                   = 0;
                /// Fetch immutable component pointers for a list of entities.
                virtual auto fetch(const std::vector<EntityUid>& components) const -> std::vector<const Component*> = 0;
                /// Fetch mutable component pointers for a list of entities.
                virtual auto fetch_mut(const std::vector<EntityUid>& components) -> std::vector<Component*>;

                /// Fetch one immutable component pointer for an entity.
                virtual auto fetch(EntityUid entity) const -> const Component* = 0;
                /// Fetch one mutable component pointer for an entity.
                virtual auto fetch_mut(EntityUid entity) -> Component*;

                /// Attach this store's components to any matching bundles.
                virtual auto assign_bundles(std::vector<Bundle>& bundles) -> void = 0;
        };

        /// Pool-backed storage for one concrete component type.
        ///
        /// Components are default-constructed when EcsWorld creates an entity whose
        /// Query includes this component type. The store maps EntityUid -> pool borrow.
        template <typename T>
        class ComponentStore : public ComponentStoreInterface {
            public:
                /// Construct storage for T using its ComponentGid from the registry.
                ComponentStore(const ComponentRegister& component_register) {
                    this->gid_ = component_register.component_gid<T>().value();
                }

                /// Reflection name used for this component type.
                static constexpr auto name() -> const char* {
                    return T::Meta::name;
                }

                /// Allocate and assign a default T component to an entity.
                virtual auto create(EntityUid assigned) -> void override final {
                    ZoneScoped;
                    assignment_.insert({assigned, components_.allocate()});
                }

                /// Free the T component assigned to an entity if one exists.
                virtual auto destroy(EntityUid component) -> void override final {
                    ZoneScoped;
                    if (!assignment_.contains(component)) {
                        return;
                    }
                    auto& borrow = assignment_.at(component);
                    components_.free(borrow);
                }

                /// Fetch immutable T components for entities.
                virtual auto fetch(const std::vector<EntityUid>& entities) const -> std::vector<const Component*> override final {
                    ZoneScoped;
                    std::vector<const Component*> components;
                    for (auto& entity : entities) {
                        auto& borrow = assignment_.at(entity);
                        components.emplace_back(borrow.get().value());
                    }
                    return components;
                }

                /// Fetch one immutable T component for an entity.
                virtual auto fetch(EntityUid entity) const -> const Component* override final {
                    ZoneScoped;
                    return assignment_.at(entity).get().value();
                }

                /// Assign matching T component pointers into bundles.
                virtual auto assign_bundles(std::vector<Bundle>& bundles) -> void override final {
                    ZoneScoped;
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
