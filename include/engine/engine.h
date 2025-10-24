#pragma once
#include "engine/logger.h"
#include "engine/meta_defines.h"
#include <memory>
#include <robin_map.h>

namespace ENGINE_NS {
    enum class LogNamespaces {
        CORE
    };

    class LogLocator {
        public:
            ENGINE_API LogLocator();
            ENGINE_API auto get(LogNamespaces ns) -> Logger&;

        private:
            uint64_t m_log_idx{};
            tsl::robin_map<LogNamespaces, Logger> m_loggers{};
    };

    class Engine {
        public:
            ENGINE_API Engine();
            ENGINE_API ~Engine();

            ENGINE_API static Engine& instance();

            ENGINE_API auto run() -> void;

            LogLocator& logger = m_logger;

        private:
            ENGINE_API auto startup() -> void;
            ENGINE_API auto shutdown() -> void;

        private:
            bool m_running = false;

            LogLocator m_logger;
    };

    extern Engine* g_ENGINE;

    void crash(ErrorCode reason);
    void crash(ErrorCode reason, int line);
    void crash(ErrorCode reason, int line, const char* function);
    void crash(ErrorCode reason, int line, const char* function, const char* file);
} // namespace ENGINE_NS
