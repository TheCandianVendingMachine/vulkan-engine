#pragma once
#include "engine/meta_defines.h"
#include <cstdint>
#include <type_traits>
#include <utility>

namespace std {
    template <class Key>
    struct hash;
}

namespace ENGINE_NS {
    template <class Tag, typename T>
    class NewType {
        public:
            NewType() : m_value() {
            }
            explicit NewType(T& value) : m_value(value) {
            }
            explicit NewType(T&& value) noexcept(std::is_nothrow_move_constructible<T>::value) : m_value(std::move(value)) {
            }

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

    template <typename Tag, typename T>
    T underlying_type_impl(NewType<Tag, T>);

    template <typename T>
    using underlying_type = decltype(underlying_type_impl(std::declval<T>()));

    template <class NewType>
    struct PreIncrement {
            friend auto operator++(NewType& rhs) -> NewType& {
                using type = underlying_type<NewType>;
                rhs        = NewType(static_cast<type>(rhs) + 1);
                return rhs;
            }
    };
    template <class NewType>
    struct PostIncrement {
            friend auto operator++(NewType& lhs, int) -> NewType {
                using type = underlying_type<NewType>;
                auto old   = lhs;
                lhs        = NewType(static_cast<type>(lhs) + 1);
                return old;
            }
    };
    template <class NewType>
    struct Increment : PreIncrement<NewType>, PostIncrement<NewType> {};

    template <class NewType>
    struct PreDecrement {
            friend auto operator--(NewType& rhs) -> NewType& {
                using type = underlying_type<NewType>;
                rhs        = NewType(static_cast<type>(rhs) - 1);
                return rhs;
            }
    };
    template <class NewType>
    struct PostDecrement {
            friend auto operator--(NewType& lhs, int) -> NewType {
                using type = underlying_type<NewType>;
                auto old   = lhs;
                lhs        = NewType(static_cast<type>(lhs) - 1);
                return old;
            }
    };
    template <class NewType>
    struct Decrement : PreDecrement<NewType>, PostDecrement<NewType> {};

    template <class NewType>
    struct Lt {
            friend auto operator<(const NewType& lhs, const NewType& rhs) -> bool {
                using type = underlying_type<NewType>;
                return static_cast<type>(lhs) < static_cast<type>(rhs);
            }
    };

    template <class NewType>
    struct Lte {
            friend auto operator<=(const NewType& lhs, const NewType& rhs) -> bool {
                using type = underlying_type<NewType>;
                return static_cast<const type>(lhs) <= static_cast<const type>(rhs);
            }
    };

    template <class NewType>
    struct Gt {
            friend auto operator>(const NewType& lhs, const NewType& rhs) -> bool {
                using type = underlying_type<NewType>;
                return static_cast<const type>(lhs) > static_cast<const type>(rhs);
            }
    };

    template <class NewType>
    struct Gte {
            friend auto operator>=(const NewType& lhs, const NewType& rhs) -> bool {
                using type = underlying_type<NewType>;
                return static_cast<const type>(lhs) >= static_cast<const type>(rhs);
            }
    };
    template <class NewType>
    struct Eq {
            friend auto operator==(const NewType& lhs, const NewType& rhs) -> bool {
                using type = underlying_type<NewType>;
                return static_cast<const type>(lhs) == static_cast<const type>(rhs);
            }
    };

    template <class NewType>
    struct Neq {
            friend auto operator!=(const NewType& lhs, const NewType& rhs) -> bool {
                using type = underlying_type<NewType>;
                return static_cast<const type>(lhs) != static_cast<const type>(rhs);
            }
    };

    template <class NewType>
    struct Orderable : Lt<NewType>, Lte<NewType>, Gt<NewType>, Gte<NewType>, Eq<NewType>, Neq<NewType> {};

    template <class NewType>
    struct Hashable {
            auto hash() const noexcept -> std::size_t {
                using type       = underlying_type<NewType>;
                auto& underlying = static_cast<const NewType&>(*this);
                return std::hash<type>{}(static_cast<type>(underlying));
            }

            auto operator()(const Hashable<NewType>& rhs) const noexcept -> std::size_t {
                return rhs.hash();
            }
    };
} // namespace ENGINE_NS
