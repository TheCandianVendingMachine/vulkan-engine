#include "engine/logger.h"
#include <Tracy/Tracy.hpp>
#include <cstdio>
#include <ranges>

using namespace ::ENGINE_NS;

logger::Level logger::operator&(logger::Level lhs, logger::Level rhs) {
    return logger::Level(uint8_t(lhs) & uint8_t(rhs));
}
logger::Level logger::operator|(logger::Level lhs, logger::Level rhs) {
    return logger::Level(uint8_t(lhs) | uint8_t(rhs));
}
bool logger::operator==(logger::Level lhs, logger::Level rhs) {
    return uint8_t(lhs) == uint8_t(rhs);
}
bool logger::operator<(logger::Level lhs, logger::Level rhs) {
    return uint8_t(lhs) < uint8_t(rhs);
}
bool logger::operator<=(logger::Level lhs, logger::Level rhs) {
    return lhs < rhs || lhs == rhs;
}
bool logger::operator>(logger::Level lhs, logger::Level rhs) {
    return uint8_t(lhs) > uint8_t(rhs);
}
bool logger::operator>=(logger::Level lhs, logger::Level rhs) {
    return lhs > rhs || lhs == rhs;
}

auto logger::level_to_string(Level level) -> std::string_view {
    switch (level) {
        case Level::ERROR:
            return "ERROR";
        case Level::WARNING:
            return "WARNING";
        case Level::INFO:
            return "INFO";
        case Level::DEBUG:
            return "DEBUG";
        default:
            return "UNKNOWN";
    }
}

auto entry_to_context(fmt::format_context& ctx, const logger::Entry& entry) -> fmt::format_context::iterator {
    return fmt::format_to(ctx.out(), "[{}] ({}) {}", logger::level_to_string(entry.level), entry.owner, entry.message);
}

auto fmt::formatter<logger::Entry>::format(logger::Entry entry, fmt::format_context& ctx) const -> fmt::format_context::iterator {
    return entry_to_context(ctx, entry);
}

auto fmt::formatter<const logger::Entry&>::format(const logger::Entry& entry, fmt::format_context& ctx) const
    -> fmt::format_context::iterator {
    return entry_to_context(ctx, entry);
}

auto fmt::formatter<logger::Entry&>::format(logger::Entry& entry, fmt::format_context& ctx) const -> fmt::format_context::iterator {
    return entry_to_context(ctx, entry);
}

auto LoggerBuilder::with_identifier(std::string&& identifier) -> LoggerBuilder& {
    m_identifier = std::move(identifier);
    return *this;
}

auto LoggerBuilder::with_stream(Stream stream) -> LoggerBuilder& {
    m_streams.push_back(std::move(stream));
    return *this;
}

auto LoggerBuilder::build() -> Logger {
    return Logger(m_identifier, std::move(m_streams));
}

Logger::Logger(std::string_view identifier, std::vector<Stream>&& streams) : m_streams(std::move(streams)), m_identifier(identifier) {
}

auto Logger::last_entries(uint64_t count) const -> std::vector<const logger::Entry*> {
    if (count == 0) {
        return {};
    }
    std::size_t maxCount = std::min(m_entries.size(), size_t(count));
    std::vector<const logger::Entry*> entries(maxCount);
    uint64_t currentCount = 0;
    for (const auto& entry : std::ranges::reverse_view{m_entries}) {
        entries[maxCount - currentCount - 1] = &entry;
        currentCount += 1;
        if (currentCount >= count) {
            break;
        }
    }
    return entries;
}

auto Logger::last_entries_of(uint64_t count, logger::Level filter) const -> std::vector<const logger::Entry*> {
    if (count == 0) {
        return {};
    }
    count                 = std::min(uint64_t(m_entries.size()), count);
    auto entries          = std::vector<const logger::Entry*>(std::size_t(count));
    uint64_t currentCount = 0;
    for (const auto& entry : std::ranges::reverse_view{m_entries}) {
        if (entry.level <= filter) {
            entries[std::size_t(count - currentCount - 1)] = &entry;
            currentCount += 1;
            if (currentCount >= count) {
                break;
            }
        }
    }

    if (currentCount < count) {
        for (std::size_t idx = 0; idx < std::size_t(currentCount); idx += 1) {
            entries[idx] = entries[std::size_t(count) - currentCount + idx];
        }
        entries.resize(currentCount);
    }
    return entries;
}

auto Logger::set_index(uint64_t index) -> void {
    m_log_idx = index;
}

void Logger::append(logger::Level level, std::string&& message) {
    logger::Entry entry{m_log_idx, level, m_identifier, std::move(message)};
    for (auto stream : m_streams) {
        if (stream.level >= level) {
            fmt::print(stream.file, "{}\n", entry);
            fflush(stream.file);
        }
    }
    m_log_idx += 1;
    m_entries.push_back(std::move(entry));
}
