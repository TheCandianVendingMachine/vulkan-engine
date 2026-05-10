#pragma once
#include "engine/meta_defines.h"
#include "engine/newtype.h"

namespace ENGINE_NS {
    namespace scene {
        struct NodeUid : NewType<NodeUid, std::uint64_t>, Eq<NodeUid>, Increment<NodeUid>, Hashable<NodeUid> {};
    } // namespace scene
} // namespace ENGINE_NS

namespace std {
    template <class Key>
    struct hash;

    template <>
    struct hash<ENGINE_NS::scene::NodeUid> : ENGINE_NS::Hashable<ENGINE_NS::scene::NodeUid> {};
} // namespace std
