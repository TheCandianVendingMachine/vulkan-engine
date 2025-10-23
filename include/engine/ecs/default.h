#include "engine/ecs/component.h"
#include "engine/ecs/defines.h"
#include "engine/meta_defines.h"
#include "engine/reflection/type.h"


namespace ENGINE_NS {
    namespace ecs {
        namespace predefined {
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
