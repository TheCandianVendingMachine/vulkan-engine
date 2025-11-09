#pragma once
#include "engine/meta_defines.h"

namespace ENGINE_NS {
    void crash(ErrorCode reason);
    void crash(ErrorCode reason, int line);
    void crash(ErrorCode reason, int line, const char* function);
    void crash(ErrorCode reason, int line, const char* function, const char* file);
} // namespace ENGINE_NS
