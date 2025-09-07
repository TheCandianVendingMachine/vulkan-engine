#include <engine/logger.h>
#include <memory>
#include <robin_map.h>

enum class LogNamespaces {
    CORE
};

class LogLocator {
    public:
        LogLocator();
        auto get(LogNamespaces ns) -> Logger&;

    private:
        tsl::robin_map<LogNamespaces, Logger> m_loggers{};
};

class Engine {
    public:
        Engine();
        ~Engine();

        static Engine& instance();

        LogLocator& logger = m_logger;

    private:
        auto startup() -> void;
        auto shutdown() -> void;
    
    private:
        bool m_running = false;

        LogLocator m_logger;
};

extern std::unique_ptr<Engine> g_ENGINE;
