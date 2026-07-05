#include "engine/shared_library.h"

#ifndef _WIN32

#include <dlfcn.h>

ENGINE_NS::SharedLibrary::load(const char* path) -> ENGINE_NS::SharedLibrary {
    auto lib = SharedLibrary{};
    lib.handle_ = dlopen(path, RTLD_NOW);
    return lib;
}

ENGINE_NS::SharedLibrary::~SharedLibrary() {
    if (library_ != nullptr) {
        auto logger = Engine::instance().logger.get(LogNamespaces::CORE);
        logger.get().info("Releasing linear algebra library");
        dlclose(handle_);
        library_ = nullptr;
    }
}

auto ENGINE_NS::SharedLibrary::get_symbol(const char* name) const -> void* {
    return dlsym(handle_, name);
}

#endif
