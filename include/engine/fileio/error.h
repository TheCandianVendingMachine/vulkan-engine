#pragma once
#include "engine/meta_defines.h"

#include <cerrno>
#include <cstdint>

namespace ENGINE_NS {
    namespace fileio {
        namespace error {
            constexpr std::uint64_t MESSAGE_LENGTH    = 256;
            constexpr const char TOO_LONG_INDICATOR[] = "...";
            enum class Flag : std::uint8_t {
                UNKNOWN,
                CLOSE,
                INDICATOR,
                GET_POS,
                MOVE,
                NOT_WRITEABLE,
                NOT_READABLE,
                WRITE_ERROR,
                OPEN
            };

            namespace types {
                struct Base {
                        static constexpr std::uint64_t REASON_LENGTH = MESSAGE_LENGTH + sizeof(TOO_LONG_INDICATOR);
                        const char reason[REASON_LENGTH]             = "";
                        Base(const char* message);
                        Base(const char* message, int error_code);
                        Base(const Base& rhs);
                        Base(Base&& rhs) noexcept;
                        auto operator=(const Base& rhs) -> Base&;
                        auto operator=(Base&& rhs) noexcept -> Base&;
                };
                struct Unknown : Base {
                        inline Unknown() : Base("An unknown error as occured") {
                        }
                        inline Unknown(int error_code) : Base("An unknown error as occured", error_code) {
                        }
                };
                struct Close : Base {
                        inline Close() : Base("An error occured while closing the file") {
                        }
                };
                struct Indicator : Base {
                        inline Indicator() : Base("A file error has occured") {
                        }
                };
                struct GetPos : Base {
                        inline GetPos() : Base("An error occured while finding current position", errno) {
                        }
                };
                struct Move : Base {
                        inline Move() : Base("An error occured while setting file position", errno) {
                        }
                };
                struct NotWritable : Base {
                        inline NotWritable() : Base("The opened file handle is not writable") {
                        }
                };
                struct NotReadable : Base {
                        inline NotReadable() : Base("The opened file handle is not readable") {
                        }
                };
                struct Write : Base {
                        inline Write() : Base("An error occured while writing to file") {};
                        inline Write(int error_code) : Base("An error occured while writing to file", error_code) {};
                };
                struct Open : Base {
                        inline Open() : Base("An error occured while opening the file", errno) {
                        }
                };
            } // namespace types

            struct Error {
                    Flag error;
                    const char (&reason)[types::Base::REASON_LENGTH];
                    union Contained {
                            types::Unknown unknown;
                            types::Close close;
                            types::Indicator indicator;
                            types::GetPos get_pos;
                            types::Move move;
                            types::NotWritable not_writable;
                            types::NotReadable not_readable;
                            types::Write write;
                            types::Open open;
                            Contained();
                    } contained;
                    std::uint8_t _padding[3] = {};

                    Error(const Error& rhs);
                    Error(Error&& rhs) noexcept;

                    auto operator=(const Error& rhs) -> Error&;
                    auto operator=(Error&& rhs) noexcept -> Error&;


                    inline Error(types::Unknown error) : error(Flag::UNKNOWN), reason(contained.unknown.reason) {
                        contained.unknown = error;
                    }
                    static inline auto unknown() -> Error {
                        return Error(types::Unknown{});
                    }

                    inline Error(types::Indicator error) : error(Flag::INDICATOR), reason(contained.indicator.reason) {
                        contained.indicator = error;
                    }
                    static inline auto indicator() -> Error {
                        return Error(types::Indicator{});
                    }

                    inline Error(types::Close error) : error(Flag::CLOSE), reason(contained.close.reason) {
                        contained.close = error;
                    }
                    static inline auto close() -> Error {
                        return Error(types::Close{});
                    }

                    inline Error(types::GetPos error) : error(Flag::GET_POS), reason(contained.get_pos.reason) {
                        contained.get_pos = error;
                    }
                    static inline auto get_pos() -> Error {
                        return Error(types::GetPos{});
                    }

                    inline Error(types::Move error) : error(Flag::MOVE), reason(contained.move.reason) {
                        contained.move = error;
                    }
                    static inline auto move() -> Error {
                        return Error(types::Move{});
                    }

                    inline Error(types::NotWritable error) : error(Flag::NOT_WRITEABLE), reason(contained.not_writable.reason) {
                        contained.not_writable = error;
                    }
                    static inline auto not_writable() -> Error {
                        return Error(types::NotWritable{});
                    }

                    inline Error(types::NotReadable error) : error(Flag::NOT_READABLE), reason(contained.not_readable.reason) {
                        contained.not_readable = error;
                    }
                    static inline auto not_readable() -> Error {
                        return Error(types::NotReadable{});
                    }

                    inline Error(types::Write error) : error(Flag::WRITE_ERROR), reason(contained.write.reason) {
                        contained.write = error;
                    }
                    static inline auto write() -> Error {
                        return Error(types::Write{});
                    }
                    static inline auto write(int error_code) -> Error {
                        return Error(types::Write(error_code));
                    }

                    inline Error(types::Open error) : error(Flag::OPEN), reason(contained.open.reason) {
                        contained.open = error;
                    }
                    static inline auto open() -> Error {
                        return Error(types::Open{});
                    }
            };
        } // namespace error
    } // namespace fileio
} // namespace ENGINE_NS
