#pragma once
#include "engine/meta_defines.h"
#define VK_NO_PROTOTYPES
#include <vulkan/vulkan_core.h>

namespace ENGINE_NS {
    void vulkan_crash(VkResult error, int line, const char* function, const char* file);
    void crash(ErrorCode reason);
    void crash(ErrorCode reason, int line);
    void crash(ErrorCode reason, int line, const char* function);
    void crash(ErrorCode reason, int line, const char* function, const char* file);
    void crash(ErrorCode reason, int line, const char* function, const char* file, const char* additional_message);

} // namespace ENGINE_NS
