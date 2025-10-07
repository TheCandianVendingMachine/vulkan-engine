#pragma once
#include "engine/meta_defines.h"
#include "engine/newtype.h"

class Uid: ENGINE_NS::NewType<Uid, size_t>, ENGINE_NS::Eq<Uid>, ENGINE_NS::Hashable<Uid> {
    using NewType::NewType;
};

template<>
struct std::hash<Uid>: ENGINE_NS::Hashable<Uid> {
};
