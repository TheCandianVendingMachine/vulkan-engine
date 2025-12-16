#include "engine/fileio/error.h"
#include <cstdint>
#include <cstring>
#include <stdlib.h>
#include <type_traits>
#include <utility>

ENGINE_NS::fileio::error::types::Base::Base(const char* message) {
    std::strncpy(const_cast<char*>(reason), message, error::MESSAGE_LENGTH);
}
ENGINE_NS::fileio::error::types::Base::Base(const char* message, int error_code) {
    char* buffer            = const_cast<char*>(reason);
    const char* err_message = std::strerror(error_code);
    enum class State : std::uint8_t {
        MESSAGE,
        ERROR,
        END
    } state   = State::MESSAGE;

    auto base = 0;
    for (auto idx = 0; idx < error::MESSAGE_LENGTH && state != State::END; idx++) {
        char current = '\0';
        switch (state) {
            case State::MESSAGE:
                current = message[idx - base];
                break;
            case State::ERROR:
                current = err_message[idx - base];
                break;
            case State::END:
                std::unreachable();
        }
        if (current == '\0') {
            const char splitter[] = ": ";
            idx += sizeof(splitter) - 1;
            std::memcpy(buffer, splitter, sizeof(splitter) - 1); // remove trailing \0
            base  = idx + 1;
            state = State(static_cast<std::underlying_type_t<State>>(state) + 1);
            buffer += sizeof(splitter) - 1;
        } else {
            *buffer = current;
            buffer += 1;
        }
    }
    // because we are only iterating to `error::MESSAGE_LENGTH`, we can't buffer overflow if this condition holds
    static_assert(sizeof(reason) == REASON_LENGTH);
    if (buffer > (reason + MESSAGE_LENGTH + 1)) {
        std::strcpy(const_cast<char*>(reason) + MESSAGE_LENGTH, TOO_LONG_INDICATOR);
    } else {
        *buffer = '\0';
    }
}

ENGINE_NS::fileio::error::types::Base::Base(const Base& rhs) {
    std::memcpy(const_cast<char*>(reason), rhs.reason, Base::REASON_LENGTH);
}

ENGINE_NS::fileio::error::types::Base::Base(Base&& rhs) noexcept {
    std::memcpy(const_cast<char*>(reason), rhs.reason, Base::REASON_LENGTH);
}

auto ENGINE_NS::fileio::error::types::Base::operator=(const Base& rhs) -> Base& {
    if (&rhs != this) {
        std::memcpy(const_cast<char*>(reason), rhs.reason, Base::REASON_LENGTH);
    }
    return *this;
}

auto ENGINE_NS::fileio::error::types::Base::operator=(Base&& rhs) noexcept -> Base& {
    if (&rhs != this) {
        std::memcpy(const_cast<char*>(reason), rhs.reason, Base::REASON_LENGTH);
    }
    return *this;
}

ENGINE_NS::fileio::error::Error::Contained::Contained() {
    std::memset(this, 0, sizeof(Contained));
}

ENGINE_NS::fileio::error::Error::Error(const Error& rhs) : error(rhs.error), reason(contained.unknown.reason) {
    std::memcpy(&contained, &rhs.contained, sizeof(Contained));
}

ENGINE_NS::fileio::error::Error::Error(Error&& rhs) noexcept : error(std::move(rhs.error)), reason(contained.unknown.reason) {
    std::memcpy(&contained, &rhs.contained, sizeof(Contained));
}

auto ENGINE_NS::fileio::error::Error::operator=(const Error& rhs) -> Error& {
    if (&rhs != this) {
        error = rhs.error;
        std::memcpy(&contained, &rhs.contained, sizeof(Contained));
    }
    return *this;
}

auto ENGINE_NS::fileio::error::Error::operator=(Error&& rhs) noexcept -> Error& {
    if (&rhs != this) {
        error = std::move(rhs.error);
        std::memcpy(&contained, &rhs.contained, sizeof(Contained));
    }
    return *this;
}
