#include "engine/shared_library.h"

#ifdef _WIN32

#include <Windows.h>
#include <libloaderapi.h>

ENGINE_NS::SharedLibrary::load(const char* path) -> ENGINE_NS::SharedLibrary {
    auto lib = SharedLibrary{};
    lib.handle_ = reinterpret_cast<void*>(LoadLibraryA(path));
    return lib;
}

ENGINE_NS::SharedLibrary::~SharedLibrary() {
    if (library_ != nullptr) {
        auto logger = Engine::instance().logger.get(LogNamespaces::CORE);
        logger.get().info("Releasing linear algebra library");
        FreeLibrary(static_cast<HMODULE>(handle_));
        library_ = nullptr;
    }
}

auto ENGINE_NS::SharedLibrary::get_symbol(const char* name) const -> void* {
    return reinterpret_cast<void*>(GetProcAddress(static_cast<HMODULE>(handle_), name));
}

#endif
