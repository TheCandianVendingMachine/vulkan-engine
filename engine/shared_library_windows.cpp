#ifdef _WIN32
#include "engine/shared_library.h"
#include "engine/engine.h"

#include <Windows.h>
#include <libloaderapi.h>
#include <utility>

auto ENGINE_NS::SharedLibrary::load(const char* path) -> ENGINE_NS::SharedLibrary {
    auto lib = SharedLibrary{};
    lib.handle_ = reinterpret_cast<void*>(LoadLibraryA(path));
    return lib;
}

ENGINE_NS::SharedLibrary::SharedLibrary(SharedLibrary&& rhs) noexcept : handle_(std::exchange(rhs.handle_, nullptr)) {
}

auto ENGINE_NS::SharedLibrary::operator=(SharedLibrary&& rhs) noexcept -> SharedLibrary& {
    if (this != &rhs) {
        if (handle_ != nullptr) {
            FreeLibrary(static_cast<HMODULE>(handle_));
        }
        handle_ = std::exchange(rhs.handle_, nullptr);
    }
    return *this;
}

ENGINE_NS::SharedLibrary::~SharedLibrary() {
    if (handle_ != nullptr) {
        auto logger = Engine::instance().logger.get(LogNamespaces::CORE);
        logger.get().info("Releasing linear algebra library");
        FreeLibrary(static_cast<HMODULE>(handle_));
        handle_ = nullptr;
    }
}

auto ENGINE_NS::SharedLibrary::get_symbol(const char* name) const -> void* {
    return reinterpret_cast<void*>(GetProcAddress(static_cast<HMODULE>(handle_), name));
}

#endif
