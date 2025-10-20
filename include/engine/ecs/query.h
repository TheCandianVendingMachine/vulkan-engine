#pragma once
#include "engine/bitset.h"
#include "engine/meta_defines.h"
#include <string_view>

namespace ENGINE_NS {
namespace ecs {
struct Query {
        Bitset& query = query_;

    private:
        Bitset query_{};

        Query(Bitset&& query);
        friend class QueryBuilder;
        friend class ComponentRegister;
};

class ComponentRegister;
class QueryBuilder {
    public:
        auto select(std::string_view name) -> QueryBuilder&;
        auto build() -> Query;

    private:
        Bitset query_{};
        ComponentRegister& component_register_;

        friend class ComponentRegister;
        QueryBuilder(ComponentRegister& component_register);
};
} // namespace ecs
} // namespace ENGINE_NS
