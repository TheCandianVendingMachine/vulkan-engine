#pragma once
#include "engine/fileio/error.h"
#include "engine/meta_defines.h"
#include "engine/newtype.h"
#include <cstdint>
#include <cstdio>
#include <expected>
#include <filesystem>
#include <type_traits>
#include <vector>

namespace ENGINE_NS {
    namespace fileio {
        enum class IoMode : std::uint8_t {
            READ   = 1 << 0,
            WRITE  = 1 << 1,
            APPEND = 1 << 2
        };
        inline auto operator|(const IoMode& lhs, const IoMode& rhs) -> IoMode {
            return IoMode(static_cast<std::underlying_type_t<IoMode>>(lhs) | static_cast<std::underlying_type_t<IoMode>>(rhs));
        }
        inline auto operator&(const IoMode& lhs, const IoMode& rhs) -> IoMode {
            return IoMode(static_cast<std::underlying_type_t<IoMode>>(lhs) & static_cast<std::underlying_type_t<IoMode>>(rhs));
        }

        enum class OpenMode : std::uint8_t {
            PLAIN,
            BINARY
        };
        inline auto operator|(const OpenMode& lhs, const OpenMode& rhs) -> OpenMode {
            return OpenMode(static_cast<std::underlying_type_t<OpenMode>>(lhs) | static_cast<std::underlying_type_t<OpenMode>>(rhs));
        }
        inline auto operator&(const OpenMode& lhs, const OpenMode& rhs) -> OpenMode {
            return OpenMode(static_cast<std::underlying_type_t<OpenMode>>(lhs) & static_cast<std::underlying_type_t<OpenMode>>(rhs));
        }

        struct FileMetadata {
                std::filesystem::path path{};
                std::filesystem::file_time_type last_write{};
                auto dirty() const -> bool;
        };
        struct IoMetadata {
                IoMode io_mode{};
                OpenMode open_mode{};
        };

        struct Offset : public NewType<Offset, std::uint64_t> {};
        struct Position : public NewType<Position, std::uint64_t> {
                inline auto is_eof() const -> bool {
                    return is_eof_;
                }
                static inline auto eof() -> Position {
                    Position position{};
                    position.is_eof_ = true;
                    return position;
                }

                using NewType::NewType;

            private:
                bool is_eof_ = false;
                friend class File;
        };
        class File {
            public:
                static auto open(const std::filesystem::path& path, OpenMode open_mode, IoMode io_mode)
                    -> std::expected<File, error::Error>;


                File(File&& rhs) noexcept;
                auto operator=(File&& rhs) noexcept -> File&;
                ~File();

                [[nodiscard("not checking if operation has an error")]]
                auto length() -> std::expected<std::uint64_t, error::Error>;

                [[nodiscard("not checking if operation has an error")]]
                auto close() -> std::expected<void, error::Error>;
                [[nodiscard("not checking if operation has an error")]]
                auto flush() -> std::expected<void, error::Error>;

                [[nodiscard("not checking if operation has an error")]]
                auto position() const -> std::expected<Position, error::Error>;

                [[nodiscard("not checking if operation has an error")]]
                auto move_to_start() -> std::expected<void, error::Error>;

                [[nodiscard("not checking if operation has an error")]]
                auto move_to(Position position) -> std::expected<void, error::Error>;

                [[nodiscard("not checking if operation has an error")]]
                auto move_to_offset(Offset offset) -> std::expected<void, error::Error>;

                template <typename T, typename = std::enable_if<std::is_trivially_copyable_v<T>>::type>
                [[nodiscard("not checking if operation has an error")]]
                auto write(const T& to_write) -> std::expected<void, error::Error> {
                    if (!handle_ || moved_) {
                        return std::expected<void, error::Error>{};
                    }
                    if (!is_writable()) {
                        return std::unexpected(error::Error::not_writable());
                    }
                    if (std::fwrite(&to_write, sizeof(T), 1, handle_) != 1) {
                        if (errno < 0) {
                            return std::unexpected(error::Error::write(errno));
                        } else {
                            return std::unexpected(error::Error::write());
                        }
                    }
                    return std::expected<void, error::Error>{};
                }
                template <typename T, typename = std::enable_if<std::is_trivially_copyable_v<T>>::type>
                [[nodiscard("not checking if operation has an error")]]
                auto write_buffer(const std::vector<T>& to_write) -> std::expected<void, error::Error> {
                    if (!handle_ || moved_) {
                        return std::expected<void, error::Error>{};
                    }
                    if (!is_writable()) {
                        return std::unexpected(error::Error::not_writable());
                    }
                    if (std::fwrite(to_write.data(), sizeof(T), to_write.size(), handle_) != to_write.size()) {
                        if (errno < 0) {
                            return std::unexpected(error::Error::write(errno));
                        } else {
                            return std::unexpected(error::Error::write());
                        }
                    }
                    return std::expected<void, error::Error>{};
                }

                template <typename TInner, typename = std::enable_if<std::is_trivially_copyable_v<TInner>>::type>
                [[nodiscard("not checking if operation has an error")]]
                auto read(std::size_t count) -> std::expected<std::vector<TInner>, error::Error> {
                    auto buffer = std::vector<TInner>(count);
                    if (auto err = read_into(buffer); !err) {
                        return std::unexpected(err.error());
                    }
                    return buffer;
                }
                template <typename TInner, typename = std::enable_if<std::is_trivially_copyable_v<TInner>>::type>
                [[nodiscard("not checking if operation has an error")]]
                auto read_into(std::vector<TInner>& buffer) -> std::expected<void, error::Error> {
                    if (!handle_ || moved_) {
                        return std::expected<void, error::Error>{};
                    }
                    if (!is_readable()) {
                        return std::unexpected(error::Error::not_readable());
                    }
                    std::fread(buffer.data(), sizeof(TInner), buffer.size(), handle_);
                    if (std::ferror(handle_)) {
                        return std::unexpected(error::Error::indicator());
                    }
                    return std::expected<void, error::Error>{};
                }

                auto is_writable() const -> bool;
                auto is_readable() const -> bool;

                const FileMetadata& metadata = metadata_;

            protected:
                File(IoMetadata io_metadata, FileMetadata file_metadata, std::FILE* handle);
                IoMetadata io_metadata_{};
                FileMetadata metadata_{};
                std::FILE* handle_ = nullptr;

            private:
                bool moved_ = false;
        };
    } // namespace fileio
} // namespace ENGINE_NS
