#pragma once
#include "engine/meta_defines.h"
#include <Tracy/Tracy.hpp>
#include <atomic>
#include <cstdint>
#include <type_traits>

namespace ENGINE_NS {
    template <typename T>
    class RwDataMut {
        public:
            ~RwDataMut() {
                ZoneScoped;
                lock_.store(false, std::memory_order_release);
            }

            auto get() -> T& {
                return wrapped_;
            }
            auto get() const -> const T& {
                return wrapped_;
            }

        private:
            template <typename T>
            friend class RwLock;
            RwDataMut(std::atomic<bool>& lock, T& wrapped) : lock_(lock), wrapped_(wrapped) {
                ZoneScoped;
            }

            std::atomic<bool>& lock_;
            T& wrapped_;
    };

    template <typename T>
    class RwData {
        public:
            ~RwData() {
                ZoneScoped;
                currently_reading_.fetch_sub(1, std::memory_order_release);
            }

            auto get() const -> const T& {
                return wrapped_;
            }

        private:
            template <typename T>
            friend class RwLock;
            RwData(std::atomic<std::uint64_t>& currently_reading, const T& wrapped) :
                currently_reading_(currently_reading), wrapped_(wrapped) {
                ZoneScoped;
                currently_reading_.fetch_add(1, std::memory_order_release);
            }

            std::atomic<std::uint64_t>& currently_reading_;
            const T& wrapped_;
    };

    template <typename T>
    class RwLock {
        public:
            template <typename = std::enable_if_t<std::is_default_constructible<T>::value>>
            RwLock() : wrapped_(T{}) {
            }
            template <typename... TArgs>
            RwLock(TArgs&&... args) : wrapped_(std::forward<TArgs>(args...)) {
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

        private:
            T wrapped_;
            std::atomic<std::uint64_t> currently_reading_{};
            std::atomic<bool> currently_writing_{};
    };
} // namespace ENGINE_NS
