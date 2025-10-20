#include "engine/ecs/query.h"
#include "engine/ecs/component.h"
#include "engine/meta_defines.h"
#include <string_view>

using namespace ::ENGINE_NS;

auto ecs::QueryBuilder::select(std::string_view name) -> ecs::QueryBuilder& {
    auto uid = this->component_register_.component_gid_by_name(name).value();
    this->query_.set(static_cast<underlying_type<decltype(uid)>>(uid));
    return *this;
}

auto ENGINE_NS::ecs::QueryBuilder::build() -> Query {
    return Query(std::move(this->query_));
}

ENGINE_NS::ecs::QueryBuilder::QueryBuilder(ComponentRegister& component_register) : component_register_(component_register) {
}

ENGINE_NS::ecs::Query::Query(Bitset&& query) : query_(std::move(query)) {
}
