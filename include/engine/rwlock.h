#pragma once
#include "engine/meta_defines.h"

#include <Tracy/Tracy.hpp>
#include <atomic>
#include <cstdint>
#include <type_traits>
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

            RwDataMut(RwDataMut<T>&& rhs) noexcept :
                lock_(std::move(rhs.lock_)), wrapped_(std::move(rhs.wrapped_)), dropped_(std::move(rhs.dropped_)) {
                rhs.moved_ = true;
            }

            auto operator=(RwDataMut<T>&& rhs) noexcept -> RwDataMut<T>& {
                if (&rhs != this) {
                    lock_      = std::move(rhs.lock_);
                    wrapped_   = std::move(rhs.wrapped_);
                    dropped_   = std::move(rhs.dropped_);
                    rhs.moved_ = true;
                }
                return *this;
            }

        private:
            template <typename T>
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
                currently_reading_(std::move(rhs.currently_reading_)), wrapped_(std::move(rhs.wrapped_)),
                dropped_(std::move(rhs.dropped_)) {
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
                    currently_reading_ = std::move(rhs.currently_reading_);
                    wrapped_           = std::move(rhs.wrapped_);
                    dropped_           = std::move(rhs.dropped_);
                    rhs.moved_         = true;
                }
                return *this;
            }

        private:
            template <typename T>
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
            template <typename = std::enable_if_t<std::is_default_constructible<T>::value>>
            RwLock() : wrapped_(T{}) {
            }

            template <typename... TArgs>
            RwLock(TArgs&&... args) : wrapped_(std::forward<TArgs>(args)...) {
            }

            template <typename = std::enable_if_t<std::is_copy_constructible<T>::value>>
            RwLock(const T& contained) : wrapped_(contained) {
            }
            template <typename = std::enable_if_t<std::is_copy_constructible<T>::value>>
            RwLock(const RwLock<T>& rhs) : wrapped_(rhs.wrapped_) {
            }
            template <typename = std::enable_if_t<std::is_move_constructible<T>::value>>
            RwLock(T&& contained) : wrapped_(std::move(contained)) {
            }
            template <typename = std::enable_if_t<std::is_move_constructible<T>::value>>
            RwLock(RwLock<T>&& rhs) noexcept : wrapped_(std::move(rhs.wrapped_)) {
                currently_reading_.store(rhs.currently_reading_.load(std::memory_order::acquire), std::memory_order::release);
                currently_writing_.store(rhs.currently_writing_.load(std::memory_order::acquire), std::memory_order::release);
                rhs.moved_ = true;
            }

            template <typename = std::enable_if_t<std::is_copy_assignable<T>::value>>
            auto operator=(const RwLock<T>& rhs) -> RwLock<T>& {
                if (&rhs != this) {
                    wrapped_ = rhs.wrapped_;
                }
                return *this;
            }
            template <typename = std::enable_if_t<std::is_move_assignable<T>::value>>
            auto operator=(RwLock<T>&& rhs) noexcept -> RwLock<T>& {
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

            template <typename = std::enable_if_t<std::is_nothrow_swappable<T>::value>>
            friend auto swap(RwLock& a, RwLock& b) noexcept -> void {
                using std::swap;
                swap(a.wrapped_, b.wrapped_);

                b.currently_reading_.exchange(a.currently_reading_.exchange(b.currently_reading_));
                b.currently_writing_.exchange(a.currently_writing_.exchange(b.currently_writing_));

                swap(a.moved_, b.moved_);
            }

        private:
            T wrapped_;
            std::atomic<std::uint64_t> currently_reading_{};
            std::atomic<bool> currently_writing_{};
            bool moved_ = false;
    };
} // namespace ENGINE_NS
