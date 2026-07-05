#pragma once
#include "engine/meta_defines.h"

#include <tracy/Tracy.hpp>
#include <atomic>
#include <cstdint>
#include <utility>

namespace ENGINE_NS {
    template <typename T>
    class RwDataMut {
        public:
            RwDataMut() = default;
            ~RwDataMut() {
                drop();
            }

            auto get() -> T& {
                return *wrapped_;
            }
            auto get() const -> const T& {
                return *wrapped_;
            }

            auto drop() -> void {
                if (moved_) {
                    return;
                }
                if (!dropped_) {
                    lock_->store(false, std::memory_order_release);
                }
                dropped_ = true;
            }

            RwDataMut(RwDataMut<T>&& rhs) noexcept : lock_(rhs.lock_), wrapped_(std::move(rhs.wrapped_)), dropped_(rhs.dropped_) {
                rhs.moved_ = true;
            }

            auto operator=(RwDataMut<T>&& rhs) noexcept -> RwDataMut<T>& {
                if (&rhs != this) {
                    lock_      = rhs.lock_;
                    wrapped_   = std::move(rhs.wrapped_);
                    dropped_   = rhs.dropped_;
                    rhs.moved_ = true;
                }
                return *this;
            }

        private:
            template <typename TLock>
            friend class RwLock;
            explicit RwDataMut(std::atomic<bool>& lock, T& wrapped) : lock_(&lock), wrapped_(&wrapped) {
            }

            std::atomic<bool>* lock_ = nullptr;
            T* wrapped_              = nullptr;
            bool dropped_            = false;
            bool moved_              = false;
    };

    template <typename T>
    class RwData {
        public:
            RwData() = default;
            ~RwData() {
                drop();
            }

            auto get() const -> const T& {
                return *wrapped_;
            }

            auto drop() -> void {
                if (moved_) {
                    return;
                }
                if (!dropped_) {
                    currently_reading_->fetch_sub(1, std::memory_order_release);
                }
                dropped_ = true;
            }

            RwData(const RwData<T>& rhs) : currently_reading_(rhs.currently_reading_), wrapped_(rhs.wrapped_), dropped_(false) {
                if (!dropped_) {
                    currently_reading_->fetch_add(1, std::memory_order_acquire);
                }
            }
            RwData(RwData<T>&& rhs) noexcept :
                currently_reading_(rhs.currently_reading_), wrapped_(std::move(rhs.wrapped_)), dropped_(rhs.dropped_) {
                rhs.moved_ = true;
            }

            auto operator=(const RwData<T>& rhs) -> RwData<T>& {
                if (&rhs != this) {
                    currently_reading_ = rhs.currently_reading_;
                    wrapped_           = rhs.wrapped_;
                    dropped_           = rhs.dropped_;

                    if (!dropped_) {
                        currently_reading_->fetch_add(1, std::memory_order_acquire);
                    }
                }
                return *this;
            }
            auto operator=(RwData<T>&& rhs) noexcept -> RwData<T>& {
                if (&rhs != this) {
                    currently_reading_ = rhs.currently_reading_;
                    wrapped_           = std::move(rhs.wrapped_);
                    dropped_           = rhs.dropped_;
                    rhs.moved_         = true;
                }
                return *this;
            }

        private:
            template <typename TLock>
            friend class RwLock;
            explicit RwData(std::atomic<std::uint64_t>& currently_reading, const T& wrapped) :
                currently_reading_(&currently_reading), wrapped_(&wrapped) {
                currently_reading_->fetch_add(1, std::memory_order_release);
            }

            std::atomic<std::uint64_t>* currently_reading_ = nullptr;
            const T* wrapped_                              = nullptr;
            bool dropped_                                  = false;
            bool moved_                                    = false;
    };

    template <typename T>
    class RwLock {
        public:
            RwLock()
                requires std::is_default_constructible_v<T>
                : wrapped_(T{}) {
            }

            template <typename... TArgs>
            RwLock(TArgs&&... args) : wrapped_(std::forward<TArgs>(args)...) {
            }


            RwLock(const T& contained)
                requires std::is_copy_constructible_v<T>
                : wrapped_(contained) {
            }

            RwLock(const RwLock<T>& rhs)
                requires std::is_copy_constructible_v<T>
                : wrapped_(rhs.wrapped_) {
            }

            RwLock(T&& contained)
                requires(std::is_move_constructible_v<T>)
                : wrapped_(std::move(contained)) {
            }

            RwLock(RwLock<T>&& rhs) noexcept
                requires std::is_move_constructible_v<T>
                : wrapped_(std::move(rhs.wrapped_)) {
                currently_reading_.store(rhs.currently_reading_.load(std::memory_order::acquire), std::memory_order::release);
                currently_writing_.store(rhs.currently_writing_.load(std::memory_order::acquire), std::memory_order::release);
                rhs.moved_ = true;
            }


            auto operator=(const RwLock<T>& rhs) -> RwLock<T>&
                requires std::is_copy_assignable_v<T>
            {
                if (&rhs != this) {
                    wrapped_ = rhs.wrapped_;
                }
                return *this;
            }

            auto operator=(RwLock<T>&& rhs) noexcept -> RwLock<T>&
                requires std::is_move_assignable_v<T>
            {
                if (&rhs != this) {
                    wrapped_ = std::move(rhs.wrapped_);
                    currently_reading_.store(rhs.currently_reading_.load(std::memory_order::acquire), std::memory_order::release);
                    currently_writing_.store(rhs.currently_writing_.load(std::memory_order::acquire), std::memory_order::release);
                    rhs.moved_ = true;
                }
                return *this;
            }

            auto read() const -> RwData<T> {
                ZoneScoped;
                while (currently_writing_.load(std::memory_order_acquire)) {
                    // spin
                }
                return RwData<T>(const_cast<std::atomic<std::uint64_t>&>(currently_reading_), wrapped_);
            }
            auto write() -> RwDataMut<T> {
                ZoneScoped;
                while (currently_reading_.load(std::memory_order_acquire) != 0) {
                    // spin
                }
                currently_writing_.store(true, std::memory_order_release);
                return RwDataMut<T>(currently_writing_, wrapped_);
            }


            friend auto swap(RwLock& a, RwLock& b) noexcept -> void
                requires std::is_nothrow_swappable_v<T>
            {
                using std::swap;
                swap(a.wrapped_, b.wrapped_);

                b.currently_reading_.exchange(a.currently_reading_.exchange(b.currently_reading_));
                b.currently_writing_.exchange(a.currently_writing_.exchange(b.currently_writing_));

                swap(a.moved_, b.moved_);
            }

            auto unsafe_open_all_locks() {
                currently_reading_.store(0, std::memory_order_release);
                currently_writing_.store(false, std::memory_order_release);
            }

        private:
            T wrapped_;
            std::atomic<std::uint64_t> currently_reading_;
            std::atomic<bool> currently_writing_;
            bool moved_ = false;
    };
} // namespace ENGINE_NS
