#include "engine/shared_library.h"

#ifdef _WIN32

#include <Windows.h>
#include <libloaderapi.h>

ENGINE_NS::load_library(const char* path) -> ENGINE_NS::LibraryHandle {
    return reinterpret_cast<void*>(LoadLibraryA(path));
}

auto ENGINE_NS::unload_library(LibraryHandle lib) -> void {
    FreeLibrary(static_cast<HMODULE>(lib));
}

auto ENGINE_NS::get_symbol(ENGINE_NS::LibraryHandle library, const char* name) -> void* {
    return reinterpret_cast<void*>(GetProcAddress(static_cast<HMODULE>(library), name));
}

#endif
