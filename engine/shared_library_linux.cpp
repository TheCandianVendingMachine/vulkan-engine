#include "engine/shared_library.h"

#ifndef _WIN32

#include <dlfcn.h>

auto ENGINE_NS::load_library(const char* path) -> ENGINE_NS::LibraryHandle {
    return dlopen(path, RTLD_NOW);
}

auto ENGINE_NS::unload_library(ENGINE_NS::LibraryHandle lib) -> void {
    dlclose(lib);
}

auto ENGINE_NS::get_symbol(ENGINE_NS::LibraryHandle library, const char* name) -> void* {
    return dlsym(library, name);
}

#endif
