#pragma once
#include "engine/meta_defines.h"
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <deque>
#include <fmt/format.h>
#include <iterator>
#include <string>
#include <string_view>
#include <vector>

namespace ENGINE_NS {
    namespace logger {
        using Clock = std::chrono::steady_clock;

        enum class Level : std::uint8_t {
            DEBUG   = 1 << 3,
            ERROR   = 1 << 2,
            WARNING = 1 << 1,
            INFO    = 1 << 0,
        };

        ENGINE_API Level operator&(Level lhs, Level rhs);
        ENGINE_API Level operator|(Level lhs, Level rhs);
        ENGINE_API bool operator==(Level lhs, Level rhs);
        ENGINE_API bool operator<(Level lhs, Level rhs);
        ENGINE_API bool operator<=(Level lhs, Level rhs);
        ENGINE_API bool operator>(Level lhs, Level rhs);
        ENGINE_API bool operator>=(Level lhs, Level rhs);

        ENGINE_API auto level_to_string(Level level) -> std::string_view;

        struct Entry {
                std::uint64_t index{};
                Level level{};
                std::string owner{};
                std::string message{};
                std::chrono::duration<double> log_time_{};
        };
    } // namespace logger

    struct Stream {
            std::FILE* file{};
            logger::Level level{};
    };

    class Logger {
        public:
            template <typename... T>
            auto debug(fmt::format_string<T...> fmt, T&&... args) -> void {
                this->log(logger::Level::DEBUG, fmt, std::forward<T>(args)...);
            }

            template <typename... T>
            auto error(fmt::format_string<T...> fmt, T&&... args) -> void {
                this->log(logger::Level::ERROR, fmt, std::forward<T>(args)...);
            }

            template <typename... T>
            auto warning(fmt::format_string<T...> fmt, T&&... args) -> void {
                this->log(logger::Level::WARNING, fmt, std::forward<T>(args)...);
            }

            template <typename... T>
            auto info(fmt::format_string<T...> fmt, T&&... args) -> void {
                this->log(logger::Level::INFO, fmt, std::forward<T>(args)...);
            }

            template <typename... T>
            auto log(logger::Level level, fmt::format_string<T...> fmt, T&&... args) -> void {
                auto message = std::string{};
                fmt::format_to(std::back_inserter(message), fmt, std::forward<T>(args)...);
                this->append(level, std::move(message));
            }

            ENGINE_API auto last_entries(uint64_t count) const -> std::vector<const logger::Entry*>;
            ENGINE_API auto last_entries_of(uint64_t count, logger::Level filter) const -> std::vector<const logger::Entry*>;

            ENGINE_API auto set_index(uint64_t index) -> void;

            friend class LoggerBuilder;

        private:
            Logger(std::string_view identifier, std::vector<Stream>&& streams);
            auto append(logger::Level level, std::string&& message) -> void;

        private:
            uint64_t m_log_idx{};
            std::chrono::time_point<logger::Clock> start_time_{};
            std::vector<Stream> m_streams{};
            std::deque<logger::Entry> m_entries{};
            std::string m_identifier{};
    };

    class LoggerBuilder {
        public:
            ENGINE_API LoggerBuilder() = default;

            ENGINE_API auto with_identifier(std::string&& identifier) -> LoggerBuilder&;
            ENGINE_API auto with_stream(Stream stream) -> LoggerBuilder&;
            ENGINE_API auto build() -> Logger;

        private:
            std::string m_identifier{};
            std::vector<Stream> m_streams{};
    };
} // namespace ENGINE_NS

template <>
class fmt::formatter<ENGINE_NS::logger::Entry> {
    public:
        constexpr auto parse(fmt::format_parse_context& ctx) {
            return ctx.begin();
        }
        auto format(ENGINE_NS::logger::Entry entry, fmt::format_context& ctx) const -> fmt::format_context::iterator;
};

template <>
class fmt::formatter<const ENGINE_NS::logger::Entry&> : fmt::formatter<ENGINE_NS::logger::Entry> {
    public:
        auto format(const ENGINE_NS::logger::Entry& entry, fmt::format_context& ctx) const -> fmt::format_context::iterator;
};

template <>
class fmt::formatter<ENGINE_NS::logger::Entry&> : fmt::formatter<ENGINE_NS::logger::Entry> {
    public:
        auto format(ENGINE_NS::logger::Entry& entry, fmt::format_context& ctx) const -> fmt::format_context::iterator;
};
