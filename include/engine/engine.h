#pragma once
#include "engine/graphics/graphics.h"
#include "engine/logger.h"
#include "engine/meta_defines.h"
#include <robin_map.h>

namespace ENGINE_NS {
    enum class LogNamespaces {
        CORE,
        GRAPHICS
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

            ENGINE_API auto quit() -> void;
            ENGINE_API auto run() -> void;

            LogLocator& logger = logger_;

        private:
            auto startup() -> void;
            auto shutdown() -> void;

            auto update() -> void;
            auto fixed_update(double update_rate) -> void;

        private:
            bool running_              = false;
            std::uint64_t frame_count_ = 0;

            LogLocator logger_;
            GraphicsEngine graphics_;
    };

    extern Engine* g_ENGINE;

} // namespace ENGINE_NS
