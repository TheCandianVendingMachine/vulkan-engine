#pragma once
#include <fmt/format.h>
#include <vector>
#include <string>
#include <deque>
#include <iterator>
#include <string_view>
#include <stdint.h>

namespace logger {
    enum class Level : uint8_t {
        DEBUG       = 1 << 3,
        ERROR       = 1 << 2,
        WARNING     = 1 << 1,
        INFO        = 1 << 0,
    };

    Level operator&(Level lhs, Level rhs);
    Level operator|(Level lhs, Level rhs);
    bool operator==(Level lhs, Level rhs);
    bool operator<(Level lhs, Level rhs);
    bool operator<=(Level lhs, Level rhs);
    bool operator>(Level lhs, Level rhs);
    bool operator>=(Level lhs, Level rhs);

    auto level_to_string(Level level) -> std::string_view;

    struct Entry {
        Level level{};
        std::string message{};
    };
}

template<> class fmt::formatter<logger::Entry> {
    public:
        constexpr auto parse(fmt::format_parse_context& ctx) {
            return ctx.begin();
        }
        auto format(logger::Entry entry, fmt::format_context& ctx) const -> fmt::format_context::iterator;
};

template<> class fmt::formatter<const logger::Entry&> : fmt::formatter<logger::Entry> {
    public:
        auto format(const logger::Entry& entry, fmt::format_context& ctx) const -> fmt::format_context::iterator;
};

template<> class fmt::formatter<logger::Entry&> : fmt::formatter<logger::Entry> {
    public:
        auto format(logger::Entry& entry, fmt::format_context& ctx) const -> fmt::format_context::iterator;
};


class Logger {
    public:
        template<typename... T>
        auto debug(fmt::format_string<T...> fmt, T&&... args) -> void {
            this->log(logger::Level::DEBUG, fmt, std::forward<T>(args)...);
        }

        template<typename... T>
        auto error(fmt::format_string<T...> fmt, T&&... args) -> void {
            this->log(logger::Level::ERROR, fmt, std::forward<T>(args)...);
        }

        template<typename... T>
        auto warning(fmt::format_string<T...> fmt, T&&... args) -> void {
            this->log(logger::Level::WARNING, fmt, std::forward<T>(args)...);
        }

        template<typename... T>
        auto info(fmt::format_string<T...> fmt, T&&... args) -> void {
            this->log(logger::Level::INFO, fmt, std::forward<T>(args)...);
        }

        template<typename... T>
        auto log(logger::Level level, fmt::format_string<T...> fmt, T&&... args) -> void{
            auto message = std::string{};
            fmt::format_to(std::back_inserter(message), fmt, std::forward<T>(args)...);
            this->append(level, std::move(message));
        }

        auto last_entries(uint64_t count) const -> std::vector<const logger::Entry*>;
        auto last_entries_of(uint64_t count, logger::Level filter) const -> std::vector<const logger::Entry*>;

    private:
        auto append(logger::Level level, std::string&& message) -> void;

    private:
        std::deque<logger::Entry> m_entries{};
};
