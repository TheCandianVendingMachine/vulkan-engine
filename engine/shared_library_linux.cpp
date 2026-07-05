#ifndef _WIN32
#include "engine/shared_library.h"
#include "engine/engine.h"

#include <dlfcn.h>

auto ENGINE_NS::SharedLibrary::load(const char* path) -> ENGINE_NS::SharedLibrary {
    auto lib = SharedLibrary{};
    lib.handle_ = dlopen(path, RTLD_NOW);
    return lib;
}

ENGINE_NS::SharedLibrary::~SharedLibrary() {
    if (handle_ != nullptr) {
        auto logger = Engine::instance().logger.get(LogNamespaces::CORE);
        logger.get().info("Releasing linear algebra library");
        dlclose(handle_);
        handle_ = nullptr;
    }
}

auto ENGINE_NS::SharedLibrary::get_symbol(const char* name) const -> void* {
    return dlsym(handle_, name);
}

#endif
