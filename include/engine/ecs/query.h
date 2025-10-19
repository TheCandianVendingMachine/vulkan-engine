#pragma once
#include "engine/bitset.h"

namespace engine {
    namespace ecs {
        struct Query {
            Bitset query_{};
        private:
            Query() = default;
            friend class QueryBuilder;
            friend class ComponentRegister;
        };

        class QueryBuilder {
        };
    }
}
 
