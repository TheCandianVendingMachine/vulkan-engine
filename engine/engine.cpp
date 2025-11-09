#include "engine/engine.h"
#include "engine/linalg/linalg.h"
#include "engine/linalg/matrix.h"
#include "engine/linalg/vector.h"
#include <SDL3/SDL.h>
#include <Tracy/Tracy.hpp>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <thread>

using namespace ::ENGINE_NS;

LogLocator::LogLocator() {
    m_loggers = {
      {LogNamespaces::CORE,     LoggerBuilder().with_identifier("ENGINE").with_stream({stdout, logger::Level::DEBUG}).build()  },
      {LogNamespaces::GRAPHICS, LoggerBuilder().with_identifier("GRAPHICS").with_stream({stdout, logger::Level::DEBUG}).build()}
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
    if (running_) {
        shutdown();
    }
}

Engine& Engine::instance() {
    return *g_ENGINE;
}

ENGINE_API auto ENGINE_NS::Engine::quit() -> void {
    if (this->running_) {
        this->running_ = false;
        this->shutdown();
    }
}

auto Engine::run() -> void {
    SDL_Event event;

    const auto tick_rate   = std::chrono::milliseconds(2);
    const auto update_rate = 1.0 / 10.0;

    double accumulator     = 0.0;
    auto last_update       = std::chrono::high_resolution_clock::now();
    auto last_frame        = std::chrono::high_resolution_clock::now();

    while (running_) {
        ++frame_count_;
        FrameMarkStart(StaticNames::EngineLoop);

        auto frame_start = std::chrono::high_resolution_clock::now();
        auto delta       = frame_start - last_update;
        accumulator += delta.count() * 1e-9;
        last_update = frame_start;

        while (SDL_PollEvent(&event) != 0) {
            switch (event.type) {
                case SDL_EVENT_QUIT:
                    this->quit();
                    break;
                default:
                    break;
            }
        }

        this->update();
        while (accumulator > 0.0) {
            FrameMarkStart(StaticNames::FixedUpdate);
            accumulator -= update_rate;
            this->fixed_update(update_rate);
            FrameMarkEnd(StaticNames::FixedUpdate);
        }

        graphics_.draw();

        FrameMarkEnd(StaticNames::EngineLoop);

        auto frame_delta = std::chrono::high_resolution_clock::now() - last_frame;
        if (frame_delta < tick_rate) {
            auto sleep = tick_rate - frame_delta;
            std::this_thread::sleep_for(sleep);
        }
        last_frame = std::chrono::high_resolution_clock::now();
        FrameMark;
    }
}
auto Engine::startup() -> void {
    ZoneScoped;
    logger.get(LogNamespaces::CORE).info("Starting");

    linalg::load_library();
    linalg::load_vector_functions(linalg::g_VECTOR_LIBRARY->library);
    linalg::load_matrix_functions(linalg::g_VECTOR_LIBRARY->library);

    graphics_.initialise();

    running_ = true;
}

auto Engine::shutdown() -> void {
    ZoneScoped;
    logger.get(LogNamespaces::CORE).info("Shutdown");

    graphics_.cleanup();

    linalg::g_VECTOR_LIBRARY->~Library();
    running_ = false;
}

auto ENGINE_NS::Engine::update() -> void {
}

auto ENGINE_NS::Engine::fixed_update(double) -> void {
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
