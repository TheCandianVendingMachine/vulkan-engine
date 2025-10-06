#pragma once
#include "engine/pool/types.h"
#include <cstdint>
#include <utility>
#include <iostream>
#include <cassert>

enum class AllocationState: std::uint8_t {
    FREE = 0,
    FIRST_FREE,
    IN_USE,
    GRAVESTONE,
};

template<typename T>
struct Allocation {
    T object;
    /*
        Axioms:
            We only allocate FIRST_FREE nodes
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
    } jump = BackwardJump(0);
    AllocationState state = AllocationState::FREE;
    std::uint8_t _padding[alignof(T) - ((sizeof(T) + sizeof(Allocation::Jump) + sizeof(AllocationState) /* sum of members */) % alignof(T))];
};

template<typename T>
class Region {
    public:
        Region() = default;
        Region(Region&& rhs) = default;
        Region(size_t capacity) {
            reserve(capacity);
        }


        class Iterator {
            public:
                using iterator_category =   std::forward_iterator_tag;
                using difference_type =     std::ptrdiff_t;
                using value_type =          Allocation<T>;
                using pointer =             value_type*;
                using reference =           value_type&;

                Iterator(pointer ptr): m_ptr(ptr) {
                }
                Iterator(const Iterator& rhs) = default;
                Iterator(const Iterator&& rhs): m_ptr(std::move(rhs.m_ptr)) {
                }

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
                        case AllocationState::FREE:
                            assert(false);
                            std::unreachable();
                        break;
                        case AllocationState::FIRST_FREE: {
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
                auto operator<(const Iterator& rhs) const -> bool = default;
                auto operator<=(const Iterator& rhs) const -> bool = default;
                auto operator>(const Iterator& rhs) const -> bool = default;
                auto operator>=(const Iterator& rhs) const -> bool = default;
                auto operator==(const Iterator& rhs) const -> bool = default;
                auto operator!=(const Iterator& rhs) const -> bool = default;

            private:
                pointer m_ptr = nullptr;
        };

        auto capacity() const -> size_t {
            return m_capacity;
        }

        auto alive() const -> bool {
            return m_capacity > 0;
        }
    
        template<typename ...TArgs>
        auto emplace(Index idx, TArgs&&... args) -> T* {
            if (static_cast<size_t>(idx) >= m_capacity) {
                return nullptr;
            }
            auto spot = get_(idx);
            if (spot->state != AllocationState::FIRST_FREE) {
                return nullptr;
            }

            switch (spot->state) {
                case AllocationState::FIRST_FREE: {
                    auto right = spot + 1;
                    if (right->state == AllocationState::GRAVESTONE || right->state == AllocationState::IN_USE) {
                        spot->state = AllocationState::IN_USE;
                    } else if (right->state == AllocationState::FREE) {
                        auto last = spot + static_cast<size_t>(spot->jump.last_free);

                        spot->state = AllocationState::IN_USE;
                        right->state = AllocationState::FIRST_FREE;
                        right->jump.last_free = spot->jump.last_free + BackwardJump(1);
                        last->jump.first_free = last->jump.first_free + ForwardJump(1);
                    } else {
                        std::unreachable();
                    }
                };
                break;
                default:
                    std::unreachable();
            }

            return new (static_cast<void*>(spot)) T(std::forward<TArgs&&>(args)...);
        }

        auto free(Index idx) {
            if (!m_pool) {
                return;
            }
            if (static_cast<size_t>(idx) > m_capacity) {
                return;
            }

            auto current = get_(idx);
            if (current->state != AllocationState::IN_USE) {
                return;
            }

            auto left = get(idx - BackwardJump(1));
            auto right = get_(idx + ForwardJump(1));
            assert(right != nullptr);

            if (left && (left->state == AllocationState::FREE || left->state == AllocationState::FIRST_FREE)) {
                current->state = AllocationState::FREE;
                if (right->state == AllocationState::FIRST_FREE) {
                    right->state = AllocationState::FREE;

                    auto last_free = right + static_cast<size_t>(right->jump.last_free);
                    last_free->jump.first_free = last_free->jump.first_free + left->jump.first_free + BackwardJump(1);

                    auto first_free = left - static_cast<size_t>(left->jump.first_free);
                    first_free->jump.last_free = first_free->jump.last_free + right->jump.last_free + ForwardJump(1);
                } else if (right->state == AllocationState::GRAVESTONE || right->state == AllocationState::IN_USE) {
                    if (left->state == AllocationState::FREE) {
                        current->jump.first_free = left->jump.first_free + BackwardJump(1);
                    } else {
                        current->jump.first_free = BackwardJump(1);
                    }
                } else {
                    std::unreachable();
                }
            } else if (!left || left->state == AllocationState::IN_USE) {
                if (right->state == AllocationState::FIRST_FREE) {
                    current->state = AllocationState::FREE;
                    right->state = AllocationState::FREE;

                    auto last_free = right + static_cast<size_t>(right->jump.last_free);
                    last_free->jump.first_free = last_free->jump.first_free + BackwardJump(1);
                    current->jump.last_free = right->jump.last_free + ForwardJump(1);
                    
                } else if (right->state == AllocationState::GRAVESTONE || right->state == AllocationState::IN_USE) {
                    current->state = AllocationState::FIRST_FREE;
                    current->jump.last_free = ForwardJump(0);
                } else {
                    std::unreachable();
                }
            } else {
                std::unreachable();
            }
        }
    
        auto reserve(size_t count) -> void {
            if (count <= m_capacity) {
                return;
            }
            auto bytes = alignof(Allocation<T>) + sizeof(Allocation<T>) * (count + 1);
            bool first_allocation = false;
            if (!m_block) {
                m_block = static_cast<std::uint8_t*>(calloc(bytes, 1));
                first_allocation = true;
            } else {
                auto original = m_block;
                m_block = static_cast<std::uint8_t*>(realloc(original, bytes));
                if (!m_block) {
                    ::free(original);
                    return;
                }
            }

            std::uint8_t* pool = m_block + alignof(Allocation<T>) - (reinterpret_cast<std::uintptr_t>(m_block) % alignof(Allocation<T>));
            if (!first_allocation) {
                auto new_bytes = sizeof(Allocation<T>) * (count - m_capacity - 1);
                memset(pool + (m_capacity + 1) * sizeof(Allocation<T>), 0, new_bytes);
            }
            m_pool = reinterpret_cast<Allocation<T>*>(pool);
            auto first = m_pool;
            auto new_first = first + m_capacity;
            auto new_end = first + count;
            new_end->state = AllocationState::GRAVESTONE;

            if (m_capacity > 0) {
                assert(new_first->state == AllocationState::GRAVESTONE);
                auto old_last_idx = Index(m_capacity - 1);
                auto old_last = get_(old_last_idx);
                switch (old_last->state) {
                    case AllocationState::FIRST_FREE: [[fallthrough]];
                    case AllocationState::FREE: {
                        auto old_first_idx = old_last_idx - old_last->jump.first_free;
                        auto old_first = get_(old_first_idx);
                        assert(old_first->state == AllocationState::FIRST_FREE);
                        old_first->jump.last_free = old_first->jump.last_free + ForwardJump(count - m_capacity);
                        new_first->state = AllocationState::FREE;

                        auto last_free_jump = ForwardJump(count - m_capacity - 1);
                        auto new_last = new_first + static_cast<size_t>(last_free_jump);
                        assert((new_last + 1)->state == AllocationState::GRAVESTONE);
                        new_last->jump.first_free = BackwardJump(static_cast<size_t>(old_first->jump.last_free));
                    };
                    break;
                    case AllocationState::IN_USE: {
                        new_first->state = AllocationState::FIRST_FREE;
                        auto last_free_jump = ForwardJump(count - m_capacity - 1);
                        new_first->jump.last_free = last_free_jump;

                        auto new_last = new_first + static_cast<size_t>(last_free_jump);
                        new_last->jump.first_free = BackwardJump(count - m_capacity - 1);
                    };
                    break;
                    default:
                        std::unreachable();
                }
            } else {
                new_first->state = AllocationState::FIRST_FREE;
                auto last_free_jump = ForwardJump(count - m_capacity - 1);
                new_first->jump.last_free = last_free_jump;

                auto new_last = new_first + static_cast<size_t>(last_free_jump);
                new_last->jump.first_free = BackwardJump(count - m_capacity - 1);
            }

            m_capacity = count;
        }

        auto index_of(void* ptr) -> Index {
            if (!ptr) {
                return Index::gravestone();
            }
            if (!m_pool) {
                return Index::gravestone();
            }
            if (ptr < m_pool) {
                return Index::gravestone();
            }
            auto idx = Index(reinterpret_cast<std::uintptr_t>(ptr) - reinterpret_cast<std::uintptr_t>(m_pool));
            if (static_cast<size_t>(idx) >= m_capacity) {
                return Index::gravestone();
            }
            return idx;
        }

        auto get(Index idx) const -> const Allocation<T>* {
            if (!m_pool) {
                return nullptr;
            }
            if (static_cast<size_t>(idx) >= m_capacity) {
                return nullptr;
            }
            auto allocation = get_(idx);
            if (allocation->state != AllocationState::IN_USE) {
                return nullptr;
            }
            return allocation;
        }
        auto get(Index idx) -> Allocation<T>* {
            if (!m_pool) {
                return nullptr;
            }
            if (static_cast<size_t>(idx) >= m_capacity) {
                return nullptr;
            }
            auto allocation = get_(idx);
            if (allocation->state != AllocationState::IN_USE) {
                return nullptr;
            }
            return allocation;
        }

        auto clear() -> void {
            if (!this->m_block) {
                return;
            }
            for (auto& allocation : *this) {
                allocation.object.~T();
            }
            ::free(this->m_block);
            this->m_block = nullptr;
            this->m_pool = nullptr;
            this->m_capacity = 0;
        }

        ~Region() {
            clear();
        }

        static auto operator delete(void* ptr, size_t size) -> void {
            auto region = static_cast<Region<T>*>(ptr);
            region->clear();
        }

        auto begin() -> Region<T>::Iterator {
            auto first = m_pool;
            switch (first->state) {
                case AllocationState::FREE: {
                    assert(false);
                    std::unreachable();
                }
                break;
                case AllocationState::FIRST_FREE: {
                    first += static_cast<size_t>(first->jump.last_free + ForwardJump(1));
                }
                break;
                default: {
                }
                break;
            }
            assert(first->state == AllocationState::IN_USE || first->state == AllocationState::GRAVESTONE);
            return Region<T>::Iterator(first);
        }

        auto end() -> Region<T>::Iterator {
            auto last = m_pool + m_capacity;
            assert(last->state == AllocationState::GRAVESTONE);
            return Region<T>::Iterator(last);
        }

        // Run integrity checks to ensure all axioms hold
        // Used exclusively for debugging
        auto do_axioms_hold_() -> bool {
            if (!m_pool) {
                assert(m_pool);
                return false;
            }
            auto allocation = m_pool;
            // Gravestone required at the end of the pool
            if ((allocation + m_capacity)->state != AllocationState::GRAVESTONE) {
                assert((allocation + m_capacity)->state == AllocationState::GRAVESTONE);
                return false;
            }
            
            bool in_free_block = false;
            while (allocation->state != AllocationState::GRAVESTONE) {
                // Only one gravestone allowed, only at end
                if (allocation->state == AllocationState::GRAVESTONE) {
                    assert(allocation->state != AllocationState::GRAVESTONE);
                    return false;
                }

                /*
                    We must only have one FIRST_FREE within a contigious allocation block
                    The allocation at the jump offset must be FREE
                    The allocation one past the jump offset must be FREE
                    The allocation at the jump offset must have a backwards jump that equals the current allocation
                */
                if (allocation->state == AllocationState::FIRST_FREE) {
                    // We must only have one FIRST_FREE within a contigious allocation block
                    if (in_free_block) {
                        assert(!in_free_block);
                        return false;
                    }
                    in_free_block = true;

                    // The allocation at the jump offset must be FREE
                    auto last_free = allocation + static_cast<size_t>(allocation->jump.last_free);
                    if (last_free->state != AllocationState::FREE && last_free->state != AllocationState::FIRST_FREE) {
                        assert(last_free->state == AllocationState::FREE || last_free->state == AllocationState::FIRST_FREE);
                        return false;
                    }

                    // The allocation one past the jump offset must be FREE
                    auto next_after_last = last_free + 1;
                    if (next_after_last->state != AllocationState::IN_USE && next_after_last->state != AllocationState::GRAVESTONE) {
                        assert(next_after_last->state == AllocationState::IN_USE && next_after_last->state == AllocationState::GRAVESTONE);
                        return false;
                    }

                    // The allocation at the jump offset must have a backwards jump that equals the current allocation
                    auto offset_first_free = last_free - static_cast<size_t>(allocation->jump.first_free);
                    if (offset_first_free != allocation) {
                        assert(offset_first_free == allocation);
                        return false;
                    }
                }

                /*
                    We must be in a FREE block
                    The allocation one past FREE must not be FIRST_FREE
                */
                if (allocation->state == AllocationState::FREE) {
                    // We must be in a FREE block
                    if (!in_free_block) {
                        assert(in_free_block);
                        return false;
                    }

                    // The allocation one past FREE must not be FIRST_FREE
                    auto next_alloc = allocation + 1;
                    if (next_alloc->state == AllocationState::FIRST_FREE) {
                        assert(next_alloc->state == AllocationState::FIRST_FREE);
                        return false;
                    }
                }

                // The allocation past IN_USE must be not be FREE
                if (allocation->state == AllocationState::IN_USE) {
                    in_free_block = false;
                    auto next_alloc = allocation + 1;
                    if (next_alloc->state == AllocationState::FREE) {
                        assert(next_alloc->state == AllocationState::FREE);
                        return false;
                    }
                }

                allocation += 1;
            }

            return true;
        }

    private:
        // The entire block of memory we allocated, including any alignment padding
        std::uint8_t* m_block = nullptr;

        // The block of memory which we use as the pool. Aligned to alignof(T)
        Allocation<T>* m_pool = nullptr;

        // How many objects are allocated
        size_t m_capacity = 0;

        auto get_(Index idx) const -> Allocation<T>* {
            return m_pool + static_cast<size_t>(idx);
        }
};
