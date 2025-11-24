#include "engine/engine_utils.h"
#include "engine/engine.h"
#include "engine/logger.h"
#include "engine/meta_defines.h"
#include <cstring>
#include <fmt/format.h>


void ENGINE_NS::vulkan_crash(VkResult error, int line, const char* function, const char* file) {
    const char* error_message = "Could not find error message";
    switch (error) {
        case VK_NOT_READY:
            error_message = "Not ready";
            break;
        case VK_TIMEOUT:
            error_message = "Operation timed out";
            break;
        case VK_EVENT_SET:
            error_message = "Event set";
            break;
        case VK_EVENT_RESET:
            error_message = "Event reset";
            break;
        case VK_INCOMPLETE:
            error_message = "Incomplete";
            break;
        case VK_ERROR_OUT_OF_HOST_MEMORY:
            error_message = "Out of memory [RAM]";
            break;
        case VK_ERROR_OUT_OF_DEVICE_MEMORY:
            error_message = "Out of memory [VRAM]";
            break;
        case VK_ERROR_INITIALIZATION_FAILED:
            error_message = "Initialisation failed";
            break;
        case VK_ERROR_DEVICE_LOST:
            error_message = "Device lost";
            break;
        case VK_ERROR_MEMORY_MAP_FAILED:
            error_message = "Failed to memory map";
            break;
        case VK_ERROR_LAYER_NOT_PRESENT:
            error_message = "Layer not present";
            break;
        case VK_ERROR_EXTENSION_NOT_PRESENT:
            error_message = "Extension not present";
            break;
        case VK_ERROR_FEATURE_NOT_PRESENT:
            error_message = "Feature not present";
            break;
        case VK_ERROR_INCOMPATIBLE_DRIVER:
            error_message = "Driver is incompatible";
            break;
        case VK_ERROR_TOO_MANY_OBJECTS:
            error_message = "Too many objects";
            break;
        case VK_ERROR_FORMAT_NOT_SUPPORTED:
            error_message = "Format is not supported";
            break;
        case VK_ERROR_FRAGMENTED_POOL:
            error_message = "Pool has become fragmented";
            break;
        case VK_ERROR_VALIDATION_FAILED:
            error_message = "Validation failed";
            break;
        case VK_ERROR_OUT_OF_POOL_MEMORY:
            error_message = "Out of memory [POOL]";
            break;
        case VK_ERROR_INVALID_EXTERNAL_HANDLE:
            error_message = "Invalid external handle";
            break;
        case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:
            error_message = "Invalid opaque capture address";
            break;
        case VK_ERROR_FRAGMENTATION:
            error_message = "Fragmentation has occured";
            break;
        case VK_PIPELINE_COMPILE_REQUIRED:
            error_message = "Pipeline has not been compiled and is required";
            break;
        case VK_ERROR_NOT_PERMITTED:
            error_message = "Not permitted";
            break;
        case VK_ERROR_SURFACE_LOST_KHR:
            error_message = "Surface lost";
            break;
        case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
            error_message = "Window already in use";
            break;
        case VK_SUBOPTIMAL_KHR:
            error_message = "Suboptimal";
            break;
        case VK_ERROR_OUT_OF_DATE_KHR:
            error_message = "Swapchain out of date";
            break;
        default:
            break;
    }

    crash(ErrorCode::VULKAN_ERROR, line, function, file, error_message);
}

void ENGINE_NS::crash(ErrorCode reason) {
    crash(reason, -1);
}

void ENGINE_NS::crash(ErrorCode reason, int line) {
    crash(reason, line, "unknown");
}

void ENGINE_NS::crash(ErrorCode reason, int line, const char* function) {
    crash(reason, line, function, "unknown");
}

void ENGINE_NS::crash(ErrorCode reason, int line, const char* function, const char* file) {
    crash(reason, line, function, file, "");
}

void ENGINE_NS::crash(ErrorCode reason, int line, const char* function, const char* file, const char* additional_message) {
    if (g_ENGINE) {
        const char* sanitized_file = file;
        char buffer[256]           = {};
        size_t idx                 = 0;
        while (true) {
            char c = sanitized_file[idx];
            if (c == '/' || c == '\\') {
                if (std::strcmp(buffer, "engine") == 0) {
                    break;
                }
                std::memset(buffer, 0, sizeof(buffer));
                sanitized_file += idx + 1;
                idx = 0;
            } else if (c == '\0') {
                sanitized_file = "";
                break;
            } else {
                buffer[idx++] = c;
            }
        }
        g_ENGINE->logger.get(LogNamespaces::CORE).error("Engine crash!");
        g_ENGINE->logger.get(LogNamespaces::CORE).error("Location: {}::{}/{}", sanitized_file, function, line);
        g_ENGINE->logger.get(LogNamespaces::CORE).error("{}", g_ERROR_CODE_STR[static_cast<std::size_t>(reason)]);
        if (*additional_message != '\0') {
            g_ENGINE->logger.get(LogNamespaces::CORE).error("Message: {}", additional_message);
        }
        g_ENGINE->shutdown();
    }
    std::exit(static_cast<int>(reason));
}
