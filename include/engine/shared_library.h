#pragma once
#include "engine/meta_defines.h"
#include "engine/engine_utils.h"

#define VERIFY(function)                                                                                                                   \
    if (!function) {                                                                                                                       \
        engine::crash(ErrorCode::CANNOT_LOAD_LINEAR_ALGEBRA_FUNCTION, __LINE__, __func__, __FILE__);                                       \
    }

namespace ENGINE_NS {
    using LibraryHandle = void*;

    auto load_library(const char* path) -> LibraryHandle;
    auto unload_library(LibraryHandle) -> void;

    template<typename TFunc>
    auto get_symbol(LibraryHandle library, const char* name) -> void*;
    template<typename TFunc>
    auto get_symbol(LibraryHandle library, TFunc& function, const char* name) -> void {
        function = reinterpret_cast<TFunc>(engine::get_symbol(library, name));
        VERIFY(function);
    }

}

