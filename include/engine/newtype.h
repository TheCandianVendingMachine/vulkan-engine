#pragma once
#include <type_traits>

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
