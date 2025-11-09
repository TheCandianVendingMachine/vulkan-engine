#include "engine/engine_utils.h"
#include "engine/engine.h"
#include "engine/logger.h"
#include "engine/meta_defines.h"
#include <cstring>
#include <fmt/format.h>

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
            } else {
                buffer[idx++] = c;
            }
        }
        g_ENGINE->logger.get(LogNamespaces::CORE).error("Engine crash!");
        g_ENGINE->logger.get(LogNamespaces::CORE).error("Location: {}::{}/{}", sanitized_file, function, line);
        g_ENGINE->logger.get(LogNamespaces::CORE).error("{}", g_ERROR_CODE_STR[static_cast<std::size_t>(reason)]);
    }
    std::exit(static_cast<int>(reason));
}
