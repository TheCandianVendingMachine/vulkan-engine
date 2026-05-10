#pragma once
#include "engine/engine_utils.h"
#include "engine/graphics/graphics.h"
#include "engine/logger.h"
#include "engine/meta_defines.h"
#include "engine/rwlock.h"
#include "engine/state/manager.h"

#include <array>

namespace ENGINE_NS {
    enum class LogNamespaces : std::uint8_t {
        CORE = 0,
        GRAPHICS,
        VULKAN,
        VULKAN_VALIDATION,
        VULKAN_PERFORMANCE,
        GAMESTATE,
        GAME,
        COUNT
    };

    class LogLocator {
        public:
            ENGINE_API LogLocator();
            ENGINE_API auto get(LogNamespaces ns) -> RwDataMut<Logger>;
            ENGINE_API auto get(LogNamespaces ns) const -> RwData<Logger>;

            auto imgui() -> void;

        private:
            friend class Engine;
            uint64_t m_log_idx{};
            std::array<RwLock<Logger>, static_cast<std::uint8_t>(LogNamespaces::COUNT)> loggers_;
            bool is_log_open_ = false;
    };

    class Engine {
        public:
            ENGINE_API Engine();
            ENGINE_API ~Engine();

            ENGINE_API static Engine& instance();

            ENGINE_API auto quit() -> void;
            ENGINE_API auto run() -> void;

            StateManager state_manager{};
            LogLocator logger{};

        private:
            auto main_loop() -> void;

            auto startup() -> void;
            auto shutdown() -> void;

            auto update() -> void;
            auto fixed_update(double update_rate) -> void;

        private:
            bool running_              = false;
            std::uint64_t frame_count_ = 0;

            GraphicsEngine graphics_;

            friend auto ::ENGINE_NS::crash(ErrorCode, int, const char*, const char*, const char*) -> void;
    };

    extern Engine* g_ENGINE;

} // namespace ENGINE_NS
