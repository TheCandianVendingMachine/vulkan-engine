#pragma once
#include "engine/pool/types.h"
#include "engine/pool/region.h"
#include "robin-map/robin_map.h"
#include "robin-map/robin_set.h"
#include <cstddef>
#include <memory>
#include <vector>
#include <iterator>
#include <compare>
#include <optional>

template<typename T, size_t DefaultCount = 512, size_t GrowthFactor = 2>
class Pool;

template<typename T>
class Borrow {
    public:
        auto operator->() -> std::optional<T*> {
            return this->get();
        }
        auto get() -> std::optional<T*> {
            return this->m_owner.get(*this);
        }

        auto operator*() const -> std::optional<const T*> {
            return this->get();
        }
        auto operator->() const -> std::optional<const T*> {
            return this->get();
        }
        auto get() const -> std::optional<const T*> {
            return this->m_owner.get(*this);
        }

        auto index() -> Index {
            return this->m_owner.index_of(*this);
        }
        auto index() const -> Index {
            return this->m_owner.index_of(*this);
        }

        explicit operator T&() {
            return *this->get().value_or(nullptr);
        }

        explicit operator T const&() const {
            return *this->get().value_or(nullptr);
        }
    
    private:
        Borrow(Handle handle, Pool<T>& pool): m_owner(pool), m_handle(handle) {}

        Pool<T>& m_owner;
        Handle m_handle;

        template<typename, size_t, size_t>
        friend class Pool;
};

template<typename T, size_t DefaultCount, size_t GrowthFactor>
class Pool {
    public:
        class Iterator {
            public:
                using iterator_category =   std::forward_iterator_tag;
                using difference_type =     std::ptrdiff_t;
                using value_type =          T;
                using pointer =             value_type*;
                using reference =           value_type&;

                Iterator(Region<T>::Iterator region_iterator): m_iterator(region_iterator) {
                }

                auto operator*() -> reference {
                    return m_iterator->object;
                }
                auto operator*() const -> reference {
                    return m_iterator->object;
                }

                auto operator->() -> pointer {
                    return &m_iterator->object;
                }
                auto operator->() const -> pointer {
                    return &m_iterator->object;
                }

                auto operator++() -> Iterator& {
                    m_iterator++;
                    return *this;
                }
                auto operator++(int) -> Iterator& {
                    auto temp = *this;
                    ++(*this);
                    return temp;
                }

                auto operator<=>(const Iterator& rhs) const {
                    return m_iterator <=> rhs.m_iterator;
                }
                auto operator<(const Iterator& rhs) const -> bool = default;
                auto operator<=(const Iterator& rhs) const -> bool = default;
                auto operator>(const Iterator& rhs) const -> bool = default;
                auto operator>=(const Iterator& rhs) const -> bool = default;
                auto operator==(const Iterator& rhs) const -> bool = default;
                auto operator!=(const Iterator& rhs) const -> bool = default;

            private:
                Region<T>::Iterator m_iterator;
        };

        Pool() = default;
        Pool(Pool&& rhs) = default;
        auto operator=(Pool&& rhs) -> Pool& = default;

        Pool(size_t initial_count) {
            this->reserve(initial_count);
        }

        auto reserve(size_t count) -> void {
            if (count <= this->capacity()) {
                return;
            }
            m_region.reserve(count);
            m_handles.reserve(count);
        }

        template<typename ...TArgs>
        auto allocate(TArgs&&... args) -> Borrow<T> {
            if (!m_region.alive()) {
                this->reserve(DefaultCount);
            }
            if (m_region.get_free_index() == Index::gravestone()) {
                this->reserve(this->m_size * GrowthFactor);
            }

            auto this_handle = m_current_handle++;
            auto this_index = m_region.get_free_index();
            m_region.emplace(this_index, std::forward<TArgs&&>(args)...);

            m_handles.insert({ this_handle, this_index });
            m_size += 1;
            return Borrow<T>(this_handle, *this);
        }

        auto free(Borrow<T> object) {
            if (!m_region.alive()) {
                return;
            }
            if (!m_handles.contains(object.m_handle)) {
                return;
            }

            auto index = object.index();
            m_region.free(index);
            m_handles.erase(object.m_handle);
            m_size -= 1;
        }

        auto capacity() const -> size_t {
            return m_region.capacity();
        }

        auto size() const -> size_t {
            return m_size;
        }

        auto index_of(Borrow<T> object) -> Index {
            if (!m_handles.contains(object.m_handle)) {
                return Index::gravestone();
            }
            return m_handles.at(object.m_handle);
        }

        auto operator[](Index idx) -> std::optional<T*> {
            if (idx == Index::gravestone()) {
                return std::nullopt;
            }
            return std::optional<T*>(&this->m_region.get(idx)->object);
        }
        auto get(Borrow<T> object) -> std::optional<T*> {
            if (!m_handles.contains(object.m_handle)) {
                return std::nullopt;
            }            auto index = m_handles.at(object.m_handle);
            return (*this)[index];
        }

        auto operator[](Index idx) const -> std::optional<const T*> {
            if (idx == Index::gravestone()) {
                return std::nullopt;
            }
            return std::optional<const T*>(&this->m_region.get(idx)->object);
        }
        auto get(Borrow<T> object) const -> std::optional<const T*> {
            if (!m_handles.contains(object.m_handle)) {
                return std::nullopt;
            }
            auto index = m_handles.at(object.m_handle);
            return (*this)[index];
        }

        auto begin() -> Iterator {
            return Iterator(m_region.begin());
        }
        auto end() -> Iterator {
            return Iterator(m_region.end());
        }

    private:
        Region<T> m_region{};
        tsl::robin_map<Handle, Index> m_handles{};

        size_t m_size = 0;
        Handle m_current_handle{};
};
