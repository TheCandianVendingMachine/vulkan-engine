#pragma once
#include "newtype.h"
#include <limits>

struct Index: NewType<Index, size_t> {
    using NewType::NewType;
    static auto gravestone() -> Index {
        using type = underlying_type<Index>;
        return Index(std::numeric_limits<type>::max());
    }

    auto operator<=>(const Index& rhs) const {
        using type = underlying_type<Index>;
        return static_cast<type>(*this) <=> static_cast<type>(rhs);
    }

    auto operator+(const ForwardJump rhs) const -> Index {
        using lhs_type = underlying_type<Index>;
        using rhs_type = underlying_type<ForwardJump>;
        return Index(static_cast<lhs_type>(*this) + static_cast<rhs_type>(rhs));
    }

    auto operator-(const BackwardJump rhs) const -> Index {
        using lhs_type = underlying_type<Index>;
        using rhs_type = underlying_type<BackwardJump>;
        if (static_cast<rhs_type>(rhs) > static_cast<lhs_type>(*this)) {
            return Index::gravestone();
        }
        return Index(static_cast<lhs_type>(*this) - static_cast<rhs_type>(rhs));
    }
};

struct Handle: NewType<Handle, size_t> {
    using NewType::NewType;

    auto operator++(int) -> Handle {
        using type = underlying_type<Handle>;
        auto ret = *this;
        *this = std::move(Handle(static_cast<type>(*this) + 1));
        return ret;
    }
};

struct ForwardJump: NewType<ForwardJump, size_t> {
    using NewType::NewType;
    auto operator+(const ForwardJump rhs) const -> ForwardJump {
        using type = underlying_type<ForwardJump>;
        return ForwardJump(static_cast<type>(*this) + static_cast<type>(rhs));
    }
    auto operator+(const BackwardJump rhs) const -> ForwardJump {
        using lhs_type = underlying_type<ForwardJump>;
        using rhs_type = underlying_type<BackwardJump>;
        if (static_cast<rhs_type>(rhs) > static_cast<lhs_type>(*this)) {
            return ForwardJump(0);
        }
        return ForwardJump(static_cast<lhs_type>(*this) - static_cast<rhs_type>(rhs));
    }
};

struct BackwardJump: NewType<BackwardJump, size_t> {
    using NewType::NewType;
    auto operator+(const BackwardJump rhs) const -> BackwardJump {
        using type = underlying_type<BackwardJump>;
        return BackwardJump(static_cast<type>(*this) + static_cast<type>(rhs));
    }
    auto operator+(const ForwardJump rhs) const -> BackwardJump {
        using lhs_type = underlying_type<BackwardJump>;
        using rhs_type = underlying_type<ForwardJump>;
        if (static_cast<rhs_type>(rhs) > static_cast<lhs_type>(*this)) {
            return BackwardJump(0);
        }
        return BackwardJump(static_cast<lhs_type>(*this) - static_cast<rhs_type>(rhs));
    }
};
