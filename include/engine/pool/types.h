#pragma once
#include "engine/newtype.h"
#include <limits>
#include <utility>

struct ForwardJump: NewType<ForwardJump, size_t> {
    using NewType::NewType;
    friend auto operator+(const ForwardJump &lhs, const ForwardJump &rhs) -> ForwardJump {
        using type = underlying_type<ForwardJump>;
        return ForwardJump(static_cast<type>(lhs) + static_cast<type>(rhs));
    }
};

struct BackwardJump: NewType<BackwardJump, size_t> {
    using NewType::NewType;
    friend auto operator+(const BackwardJump& lhs, const BackwardJump& rhs) -> BackwardJump {
        using type = underlying_type<BackwardJump>;
        return BackwardJump(static_cast<type>(lhs) + static_cast<type>(rhs));
    }
};

inline auto operator+(const ForwardJump& lhs, const BackwardJump rhs) -> ForwardJump {
    using lhs_type = underlying_type<ForwardJump>;
    using rhs_type = underlying_type<BackwardJump>;
    if (static_cast<rhs_type>(rhs) > static_cast<lhs_type>(lhs)) {
        return ForwardJump(0);
    }
    return ForwardJump(static_cast<lhs_type>(lhs) - static_cast<rhs_type>(rhs));
}

inline auto operator+(const BackwardJump& lhs, const ForwardJump rhs) -> BackwardJump {
    using lhs_type = underlying_type<BackwardJump>;
    using rhs_type = underlying_type<ForwardJump>;
    if (static_cast<rhs_type>(rhs) > static_cast<lhs_type>(lhs)) {
        return BackwardJump(0);
    }
    return BackwardJump(static_cast<lhs_type>(lhs) - static_cast<rhs_type>(rhs));
}

struct Index: NewType<Index, size_t>, Orderable<Index>, Hashable<Index> {
    using NewType::NewType;
    static auto gravestone() -> Index {
        using type = underlying_type<Index>;
        return Index(std::numeric_limits<type>::max());
    }

    friend auto operator+(const Index &lhs, const ForwardJump &rhs) -> Index {
        using lhs_type = underlying_type<Index>;
        using rhs_type = underlying_type<ForwardJump>;
        return Index(static_cast<lhs_type>(lhs) + static_cast<rhs_type>(rhs));
    }

    friend auto operator-(const Index& lhs, const BackwardJump& rhs) -> Index {
        using lhs_type = underlying_type<Index>;
        using rhs_type = underlying_type<BackwardJump>;
        if (static_cast<rhs_type>(rhs) > static_cast<lhs_type>(lhs)) {
            return Index::gravestone();
        }
        return Index(static_cast<lhs_type>(lhs) - static_cast<rhs_type>(rhs));
    }
};

struct Handle: NewType<Handle, size_t>, Orderable<Handle>, Hashable<Handle> {
    using NewType::NewType;

    auto operator++(int) -> Handle {
        using type = underlying_type<Handle>;
        auto temp = *this;
        *this = std::move(Handle(static_cast<type>(*this) + 1));
        return temp;
    }
};

template<>
struct std::hash<Index>: Hashable<Index> {
};
template<>
struct std::hash<Handle>: Hashable<Handle> {
};
