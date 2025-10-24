#include "engine/engine.h"
#include "engine/linalg/linalg.h"
#include "engine/linalg/matrix.h"
#include "engine/linalg/vector.h"
#include <cstdlib>
#include <cstring>

using namespace ::ENGINE_NS;

LogLocator::LogLocator() {
    m_loggers = {
      {LogNamespaces::CORE, LoggerBuilder().with_identifier("ENGINE").with_stream({stdout, logger::Level::DEBUG}).build()}
    };
}

auto LogLocator::get(LogNamespaces ns) -> Logger& {
    auto& logger = m_loggers.at(ns);
    logger.set_index(m_log_idx);
    m_log_idx += 1;
    return logger;
}

Engine::Engine() {
    if (!g_ENGINE) {
        g_ENGINE = this;
        startup();
    }
}

Engine::~Engine() {
    if (m_running) {
        shutdown();
    }
}

Engine& Engine::instance() {
    return *g_ENGINE;
}

auto Engine::run() -> void {
    while (true) {
    }
}
auto Engine::startup() -> void {
    logger.get(LogNamespaces::CORE).info("Starting");
    linalg::load_library();
    linalg::load_vector_functions(linalg::g_VECTOR_LIBRARY->library);
    linalg::load_matrix_functions(linalg::g_VECTOR_LIBRARY->library);


    m_running = true;
}

auto Engine::shutdown() -> void {
    logger.get(LogNamespaces::CORE).info("Shutdown");
    linalg::g_VECTOR_LIBRARY->~Library();
    m_running = false;
}

Engine* engine::g_ENGINE = nullptr;

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
