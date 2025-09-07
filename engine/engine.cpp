#include <engine/engine.h>

LogLocator::LogLocator() {
    m_loggers = {
        {
            LogNamespaces::CORE, LoggerBuilder()
                .with_identifier("ENGINE")
                .with_stream({ stdout, logger::Level::DEBUG })
                .build()
        }
    };    
}

auto LogLocator::get(LogNamespaces ns) -> Logger& {
    return m_loggers.at(ns);
}

Engine::Engine() {
    if (!g_ENGINE) {
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

auto Engine::startup() -> void {
    logger.get(LogNamespaces::CORE).info("Starting");

    m_running = true;
}

auto Engine::shutdown() -> void {
    logger.get(LogNamespaces::CORE).info("Shutdown");
    m_running = false;
}

std::unique_ptr<Engine> g_ENGINE = std::make_unique<Engine>();
