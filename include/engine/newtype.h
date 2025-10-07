#pragma once
#include <type_traits>
#include <utility>

template<class Tag, typename T>
class NewType {
    public:
        NewType(): m_value() {}
        explicit NewType(T& value): m_value(value) {}
        explicit NewType(T&& value) noexcept(std::is_nothrow_move_constructible<T>::value): m_value(std::move(value)) {}
        
        explicit operator T&() noexcept {
            return this->m_value;
        }

        explicit operator T const&() const noexcept {
            return this->m_value;
        }

        friend auto swap(NewType& a, NewType& b) noexcept -> void {
            std::swap(static_cast<T&>(a), static_cast<T&>(b));
        }

    private:
        T m_value;
};

template<typename Tag, typename T>
T underlying_type_impl(NewType<Tag, T>);

template<typename T>
using underlying_type = decltype(underlying_type_impl(std::declval<T>()));


template<class NewType>
struct Lt {
    friend auto operator<(const NewType& lhs, const NewType& rhs) -> bool {
        using type = underlying_type<NewType>;
        return static_cast<type>(lhs) < static_cast<type>(rhs);
    }
};

template<class NewType>
struct Lte {
    friend auto operator<=(const NewType& lhs, const NewType& rhs) -> bool {
        using type = underlying_type<NewType>;
        return static_cast<const type>(lhs) <= static_cast<const type>(rhs);
    }
};

template<class NewType>
struct Gt {
    friend auto operator>(const NewType& lhs, const NewType& rhs) -> bool {
        using type = underlying_type<NewType>;
        return static_cast<const type>(lhs) > static_cast<const type>(rhs);
    }
};

template<class NewType>
struct Gte {
    friend auto operator>=(const NewType& lhs, const NewType& rhs) -> bool {
        using type = underlying_type<NewType>;
        return static_cast<const type>(lhs) >= static_cast<const type>(rhs);
    }
};
template<class NewType>
struct Eq {
    friend auto operator==(const NewType& lhs, const NewType& rhs) -> bool {
        using type = underlying_type<NewType>;
        return static_cast<const type>(lhs) == static_cast<const type>(rhs);
    }
};

template<class NewType>
struct Neq {
    friend auto operator!=(const NewType& lhs, const NewType& rhs) -> bool {
        using type = underlying_type<NewType>;
        return static_cast<const type>(lhs) != static_cast<const type>(rhs);
    }
};

template<class NewType>
struct Orderable: Lt<NewType>, Lte<NewType>, Gt<NewType>, Gte<NewType>, Eq<NewType>, Neq<NewType> {
};

template<class NewType>
struct Hashable {
    auto hash() const noexcept -> std::size_t {
        using type = underlying_type<NewType>;
        auto& underlying = static_cast<const NewType&>(*this);
        return std::hash<type>{}(static_cast<type>(underlying));
    }

    auto operator()(const Hashable<NewType>& rhs) const noexcept -> std::size_t {
        return rhs.hash();
    }
};

