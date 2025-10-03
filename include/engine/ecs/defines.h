#pragma once
#include "newtype.h"

class Uid: NewType<Uid, size_t> {
    using NewType::NewType;

    inline auto operator==(const Uid& rhs) const -> bool {
        using type = underlying_type<Uid>;
        return static_cast<const type&>(*this) == static_cast<const type&>(rhs);
    }
};

