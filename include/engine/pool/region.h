#pragma once
#include "engine/pool/types.h"
#include <cstdint>
#include <utility>

template<typename T>
struct Allocation {
    T object;
    /*
        Axioms:
            If state == FIRST_FREE =>
                - last_free will always equal the distance to the last free block in this sequence

            If state == FREE and right.state == IN_USE =>
                - first_free will always equal the distance to the first free block in this sequence
        
        These axioms let us iterate forwards from the first index without additional datastructures
        Proof:
        Case 1:
            We start iteration from first element
            Two cases
            Case 1:
                Element is FIRST FREE
                > Jump to last_free + 1, this will either be the first element or the end
                > We choose that as our index
            Case 2:
                Element is IN USE
                > Pick this as the first element
        Case 2:
            We are midway through iteration
            Two cases
            Case 1:
                Next element is FIRST FREE
                > Jump to last_free + 1, this will either be the first element of the end
                > We choose that as our next index
            Case 2:
                Next element is IN USE
                > We pick that as our next index
        
        Via the spreadsheet in supplimentary, we also prove we can keep the axioms for all valid cases of allocation and deallocation
    */
    union Jump {
        /*
            How many indices previous until we hit the first free block in this run
                state == FREE
                first_free > 0
        */
        BackwardJump first_free;

        /*
            How many indices ahead of us until the next in use allocation
                state == FIRST_FREE
                last_free >= 0
        */
        ForwardJump last_free;
    } jump = 0;
    enum class State: std::uint8_t {
        FREE = 0,
        FIRST_FREE,
        IN_USE,
        GRAVESTONE,
    } state = State::FREE;
    std::uint8_t _padding[alignof(T) - ((sizeof(T) + sizeof(Allocation::Jump) + sizeof(Allocation::State) /* sum of members */) % alignof(T))];
};

template<typename T>
class Region {
    public:
        class Iterator {
            public:
                using iterator_category =   std::forward_iterator_tag;
                using difference_type =     std::ptrdiff_t;
                using value_type =          Allocation<T>;
                using pointer =             value_type*;
                using reference =           value_type&;

                Iterator(pointer ptr): m_ptr(pointer) {
                }
                Iterator(const Iterator& rhs) = default;
                Iterator(const Iterator&& rhs) = default;

                auto operator*() -> reference {
                    return *m_ptr;
                }
                auto operator*() const -> reference {
                    return *m_ptr;
                }
                auto operator->() -> pointer {
                    return m_ptr;
                }
                auto operator->() const -> pointer {
                    return m_ptr;
                }

                auto operator++() -> Iterator& {
                    m_ptr += 1;
                    switch (m_ptr->state) {
                        case Allocation::State::FREE:
                            assert(false);
                            std::unreachable();
                        break;
                        case Allocation::State::FIRST_FREE: {
                            m_ptr += static_cast<size_t>(m_ptr->jump.last_free + ForwardJump(1));
                        }
                        break;
                        default: {
                        }
                        break;
                    }

                    return *this;
                }
                auto operator++(int) -> Iterator& {
                    auto temp = *this;
                    ++(*this);
                    return temp;
                }

                auto operator<=>(const Iterator& rhs) const {
                    return m_ptr <=> rhs.m_ptr;
                }

            private:
                pointer m_ptr = nullptr;
        };

        auto capacity() const -> size_t {
            return m_capacity;
        }

        auto alive() const -> bool {
            return m_capacity > 0
        }
    
        template<typename ...TArgs>
        auto emplace(Index idx, TArgs&&... args) -> T* {
            if (static_cast<size_t>(idx) >= m_capacity) {
                return nullptr;
            }
            auto spot = static_cast<Allocation<T*>>(m_pool)[static_cast<size_t>(idx)];

            // Todo:
            // Update allocations on left and right to make sure axioms hold
            assert(spot->state != Allocation<T>::State::IN_USE);
            assert(spot->state != Allocation<T>::State::GRAVESTONE);

            switch (spot->state) {
                case Allocation<T>::State::FIRST_FREE: {
                    auto right = spot + 1;
                    if (right->state == Allocation<T>::State::GRAVESTONE || right->state == Allocation<T>::State::IN_USE) {
                        
                    } else if (right->state == Allocation<T>::State::FREE) {

                    } else {
                        std::unreachable();
                    }
                };
                break;
                case Allocation<T>::State::FREE: {

                };
                break;
                default: {
                    std::unreachable();
                }
                break;
            }

            return new (static_cast<void*>(spot)) T(std::forward<TArgs&&>(args)...);
        }

        auto free(Index idx) {
            if (static_cast<size_t>(idx) >= m_capacity) {
                return;
            }
            // Todo:
            // Update allocations on left and right to make sure axioms hold
        }
    
        auto reserve(size_t count) -> void {
            if (count <= m_capacity) {
                return;
            }
            auto bytes = alignof(Allocation<T>) + sizeof(Allocation<T>) * (count + 1);
            if (!m_block) {
                m_block = calloc(bytes, 1);
            } else {
                m_block = realloc(m_block, bytes);
            }

            m_pool = m_block + alignof(Allocation<T>) - (m_block % alignof(Allocation<T>));

            auto new_first = static_cast<Allocation<T>*>(m_pool) + m_capacity;
            assert(new_first->state == Allocation<T>::State::GRAVESTONE);
            if (m_capacity > 0) {
                auto old_last_idx = Index(m_capacity - 1);
                auto old_last = static_cast<Allocation<T>*>(m_pool) + static_cast<size_t>(old_last_idx);
                switch (old_last->state) {
                    case Allocation<T>::State::FIRST_FREE: [[fallthrough]]
                    case Allocation<T>::State::FREE: {
                        auto first_jump_idx = old_last_idx - old_last->jump.first_free;
                        auto old_first = static_cast<Allocation<T>*>(m_pool + static_cast<size_t>(first_jump_idx));
                        old_first->jump.last_free = old_first->jump.last_free + ForwardJump(count - m_capacity);
                        new_first->state = Allocation<T>::State::FREE;
                    };
                    case Allocation<T>::State::IN_USE: {
                        new_first->state = Allocation<T>::State::FIRST_FREE;
                        new_first->jump.last_free = ForwardJump(count - 1);

                        auto new_last = new_first + count - 1;
                        new_last->first_free = BackwardJump(count - 1);
                    };
                }
            } else {
                new_first->state = Allocation<T>::State::FIRST_FREE;
                new_first->jump.last_free = ForwardJump(count - 1);

                auto new_last = new_first + count - 1;
                new_last->first_free = BackwardJump(count - 1);
            }

            m_capacity = count;
        }

        auto index_of(void* ptr) -> Index {
            if (ptr < pool) {
                return Index::gravestone();
            }
            auto idx = Index(ptr - m_pool);
            if (idx >= m_capacity) {
                return Index::gravestone();
            }
            return idx;
        }

        auto get(Index idx) const -> const Allocation<T>* {
            if (!m_pool) {
                return nullptr;
            }
            if (idx >= m_capacity) {
                return nullptr;
            }
            return &(static_cast<Allocation<T>*>(m_pool)[static_cast<size_t>(idx)]);
        }
        auto get(Index idx) -> Allocation<T>* {
            if (!m_pool) {
                return nullptr;
            }
            if (idx >= m_capacity) {
                return nullptr;
            }
            return &(static_cast<Allocation<T>*>(m_pool)[static_cast<size_t>(idx)]);
        }

        static auto operator delete(void* ptr, size_t size) -> void {
            auto region = static_cast<Region<T>*>(ptr);
            if (!region->m_block) {
                return;
            }
            free(region->m_block);
            region->m_block = nullptr;
            region->m_pool = nullptr;
            region->m_capacity = 0;
        }

        auto begin() -> Region<T>::Iterator {
            auto first = &static_cast<Allocation*>(m_pool);
            switch (first->state) {
                case Allocation::State::FREE: {
                    assert(false);
                    std::unreachable();
                }
                break;
                case Allocation::State::FIRST_FREE: {
                    first += static_cast<size_t>(m_ptr->jump.last_free + 1);
                }
                break;
                default: {
                }
                break;
            }
            assert(first->state == Allocation::State::IN_USE);
            return Region<T>::Iterator(first);
        }

        auto end() -> Region<T>::Iterator {
            auto last = static_cast<Allocation*>(m_pool) + m_capacity;
            assert(laste->state == Allocation::State::GRAVESTONE);
            return Region<T>::Iterator(last);
        }

        // Run integrity checks to ensure all axioms hold
        // Used exclusively for debugging
        auto do_axioms_hold_() -> bool {
            auto allocation = static_cast<Allocation<T>*>(m_pool);
            // Gravestone required at the end of the pool
            if ((allocation + m_capacity)->state != Allocation<T>::GRAVESTONE) {
                assert((allocation + m_capacity)->state == Allocation<T>::GRAVESTONE);
                return false;
            }
            
            bool in_free_block = false;
            while (allocation->state != Allocation<T>::GRAVESTONE) {
                // Only one gravestone allowed, only at end
                if (allocation->state == Allocation<T>::GRAVESTONE) {
                    assert((allocation + m_capacity)->state != Allocation<T>::GRAVESTONE);
                    return false;
                }

                /*
                    We must only have one FIRST_FREE within a contigious allocation block
                    The allocation at the jump offset must be FREE
                    The allocation one past the jump offset must be FREE
                    The allocation at the jump offset must have a backwards jump that equals the current allocation
                */
                if (allocation->state == Allocation<T>::FIRST_FREE) {
                    if (in_free_block) {
                        assert(!in_free_block);
                        return false;
                    }
                    in_free_block = true;
                    auto last_free = allocation + static_cast<size_t>(allocation->jump.last_free);
                    if (last_free->state != Allocation<T>::FREE) {
                        assert(last_free->state == Allocation<T>::FREE);
                        return false;
                    }
                    if ((last_free + 1)->state != Allocation<T>::IN_USE) {
                        assert((last_free + 1)->state == Allocation<T>::FREE);
                        return false;
                    }
                    auto offset_first_free = allocation - static_cast<size_t>(allocation->jump.first_free);
                    if (offset_first_free != allocation) {
                        assert(offset_first_free == allocation);
                        return false;
                    }
                }

                /*
                    We must be in a FREE block
                    The allocation one past FREE must not be FIRST_FREE
                */
                if (allocation->state == Allocation<T>::FREE) {
                    if (!in_free_block) {
                        assert(in_free_block);
                        return false;
                    }
                    auto next_alloc = allocation + 1;
                    if (next_alloc->state == Allocation<T>::FIRST_FREE) {
                        assert(next_alloc->state == Allocation<T>::FIRST_FREE);
                        return false;
                    }
                }

                // The allocation past IN_USE must be not be FREE
                if (allocation->state == Allocation<T>::IN_USE) {
                    in_free_block = false;
                    auto next_alloc = allocation + 1;
                    if (next_alloc->state == Allocation<T>::FREE) {
                        assert(next_alloc->state == Allocation<T>::FREE);
                        return false;
                    }
                }
            }

            return true;
        }

    private:
        // The entire block of memory we allocated, including any alignment padding
        std::uint8_t* m_block = nullptr;

        // The block of memory which we use as the pool. Aligned to alignof(T)
        std::uint8_t* m_pool = nullptr;

        // How many objects are allocated
        size_t m_capacity = 0;
};
