#include "engine/fileio/file.h"

#include "engine/fileio/error.h"

#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <type_traits>

auto ENGINE_NS::fileio::File::open(const std::filesystem::path& path, OpenMode open_mode, IoMode io_mode)
    -> std::expected<File, error::Error> {
    char mode[] = "\0\0\0";

    auto end = 1;
    if ((io_mode & IoMode::APPEND) == IoMode::APPEND) {
        mode[0] = 'a';
    } else {
        auto write_set = (io_mode & IoMode::WRITE) == IoMode::WRITE;
        auto read_set  = (io_mode & IoMode::READ) == IoMode::READ;

        if (write_set && !read_set) {
            mode[0] = 'w';
        } else if (!write_set && read_set) {
            mode[0] = 'r';
        } else {
            mode[0] = 'w';
            mode[1] = '+';
            end     = 2;
        }
    }

    switch (open_mode) {
        case OpenMode::BINARY:
            mode[end] = 'b';
            break;
        default:
            break;
    }

    std::FILE* handle = std::fopen(reinterpret_cast<const char*>(path.u8string().c_str()), mode);
    if (!handle) {
        return std::unexpected(error::Error::open());
    }

    IoMetadata io_metadata{io_mode, open_mode};
    FileMetadata file_metadata{};
    file_metadata.path       = path;
    file_metadata.last_write = std::filesystem::last_write_time(path);

    return File(io_metadata, file_metadata, handle);
}

ENGINE_NS::fileio::File::File(File&& rhs) noexcept :
    io_metadata_(std::move(rhs.io_metadata_)), metadata_(std::move(rhs.metadata_)), handle_(std::move(rhs.handle_)) {
    rhs.moved_ = true;
}

auto ENGINE_NS::fileio::File::operator=(File&& rhs) noexcept -> File& {
    if (&rhs != this) {
        io_metadata_ = std::move(rhs.io_metadata_);
        metadata_    = std::move(rhs.metadata_);
        handle_      = std::move(rhs.handle_);
        rhs.moved_   = true;
    }
    return *this;
}

ENGINE_NS::fileio::File::~File() {
    if (!handle_ || moved_) {
        return;
    }
    (void)close();
}

auto ENGINE_NS::fileio::File::length() -> std::expected<std::uint64_t, error::Error> {
    if (!handle_ || moved_) {
        return 0;
    }
    auto start = this->position();
    if (!start) {
        return std::unexpected(start.error());
    }
#ifdef _WIN32
    auto result = _fseeki64(handle_, 0, SEEK_END);
#elif __linux__
    auto result = fseeko(handle_, 0, SEEK_END);
#else
    #error "Unsupported OS"
#endif
    if (result == -1) {
        return std::unexpected(error::Error::move());
    }
    auto end = this->position();
    if (!end) {
        return std::unexpected(end.error());
    }
    if (auto err = move_to(start.value()); !err) {
        return std::unexpected(err.error());
    }
    return static_cast<std::uint64_t>(end.value());
}

auto ENGINE_NS::fileio::File::close() -> std::expected<void, error::Error> {
    if (!handle_ || moved_) {
        return std::expected<void, error::Error>{};
    }
    auto handle = handle_;
    handle_     = nullptr;
    if (0 != std::fclose(handle)) {
        return std::unexpected(error::Error::close());
    }
    return std::expected<void, error::Error>{};
}

auto ENGINE_NS::fileio::File::flush() -> std::expected<void, error::Error> {
    if (!handle_ || moved_) {
        return std::expected<void, error::Error>{};
    }
    if (0 != std::fflush(handle_)) {
        return std::unexpected(error::Error::indicator());
    }
    return std::expected<void, error::Error>{};
}

auto ENGINE_NS::fileio::File::position() const -> std::expected<Position, error::Error> {
    if (!handle_ || moved_) {
        return Position::eof();
    }
#ifdef _WIN32
    auto offset = _ftelli64(handle_);
#elif __linux__
    auto offset = ftello(handle_);
#else
    #error "Unsupported OS"
#endif
    if (offset == -1ll) {
        return std::unexpected(error::Error::get_pos());
    }
    if (std::feof(handle_)) {
        return Position::eof();
    }
    return Position{static_cast<std::uint64_t>(offset)};
}

auto ENGINE_NS::fileio::File::move_to_start() -> std::expected<void, error::Error> {
    if (!handle_ || moved_) {
        return std::expected<void, error::Error>{};
    }
    std::rewind(handle_);
    return std::expected<void, error::Error>{};
}

auto ENGINE_NS::fileio::File::move_to(Position position) -> std::expected<void, error::Error> {
    if (!handle_ || moved_) {
        return std::expected<void, error::Error>{};
    }
#ifdef _WIN32
    auto result = _fseeki64(handle_, static_cast<std::uint64_t>(position), SEEK_SET);
#elif __linux__
    auto result = fseeko(handle_, static_cast<std::uint64_t>(position), SEEK_SET);
#else
    #error "Unsupported OS"
#endif
    if (result == -1) {
        return std::unexpected(error::Error::move());
    }
    return std::expected<void, error::Error>{};
}

auto ENGINE_NS::fileio::File::move_to_offset(Offset offset) -> std::expected<void, error::Error> {
    if (!handle_ || moved_) {
        return std::expected<void, error::Error>{};
    }
#ifdef _WIN32
    auto result = _fseeki64(handle_, static_cast<std::uint64_t>(offset), SEEK_CUR);
#elif __linux__
    auto result = fseeko(handle_, static_cast<std::uint64_t>(offset), SEEK_CUR);
#else
    #error "Unsupported OS"
#endif
    if (result == -1) {
        return std::unexpected(error::Error::move());
    }
    return std::expected<void, error::Error>{};
}

auto ENGINE_NS::fileio::File::is_writable() const -> bool {
    using underlying = std::underlying_type_t<decltype(io_metadata_.io_mode)>;
    if (IoMode(static_cast<underlying>(io_metadata_.io_mode) & static_cast<underlying>(IoMode::WRITE)) == IoMode::WRITE) {
        return true;
    }
    if (IoMode(static_cast<underlying>(io_metadata_.io_mode) & static_cast<underlying>(IoMode::APPEND)) == IoMode::APPEND) {
        return true;
    }
    return false;
}

auto ENGINE_NS::fileio::File::is_readable() const -> bool {
    using underlying = std::underlying_type_t<decltype(io_metadata_.io_mode)>;
    if (IoMode(static_cast<underlying>(io_metadata_.io_mode) & static_cast<underlying>(IoMode::READ)) == IoMode::READ) {
        return true;
    }
    return false;
}

ENGINE_NS::fileio::File::File(IoMetadata io_metadata, FileMetadata file_metadata, std::FILE* handle) :
    io_metadata_(io_metadata), metadata_(file_metadata), handle_(handle) {
}

auto ENGINE_NS::fileio::FileMetadata::dirty() const -> bool {
    return false;
}
