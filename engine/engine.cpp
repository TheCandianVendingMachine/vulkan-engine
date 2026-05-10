#include "engine/engine.h"

#include "engine/linalg/linalg.h"
#include "engine/linalg/matrix.h"
#include "engine/linalg/vector.h"

#include <SDL3/SDL.h>
#include <Tracy/Tracy.hpp>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <imgui.h>
#include <thread>

using namespace ::ENGINE_NS;

LogLocator::LogLocator() :
    loggers_({
      LoggerBuilder().with_identifier("ENGINE").with_stream({stdout, logger::Level::DEBUG}).build(m_log_idx),
      LoggerBuilder().with_identifier("GRAPHICS").with_stream({stdout, logger::Level::DEBUG}).build(m_log_idx),
      LoggerBuilder().with_identifier("VULKAN").with_stream({stdout, logger::Level::DEBUG}).build(m_log_idx),
      LoggerBuilder().with_identifier("VULKAN [PERFORMANCE]").with_stream({stdout, logger::Level::DEBUG}).build(m_log_idx),
      LoggerBuilder().with_identifier("VULKAN [VALIDATION]").with_stream({stdout, logger::Level::DEBUG}).build(m_log_idx),
      LoggerBuilder().with_identifier("GAMESTATE").with_stream({stdout, logger::Level::DEBUG}).build(m_log_idx),
      LoggerBuilder().with_identifier("GAME").with_stream({stdout, logger::Level::DEBUG}).build(m_log_idx),
    }) {
}

auto LogLocator::get(LogNamespaces ns) -> RwDataMut<Logger> {
    return loggers_[static_cast<std::uint8_t>(ns)].write();
}
auto LogLocator::get(LogNamespaces ns) const -> RwData<Logger> {
    return loggers_[static_cast<std::uint8_t>(ns)].read();
}

auto ENGINE_NS::LogLocator::imgui() -> void {
    if (ImGui::Begin("Logs")) {
        std::vector<const logger::Entry*> entries = {};
        for (auto& logger : loggers_) {
            auto lock         = logger.read();
            auto& log         = lock.get();
            auto last_entries = log.last_entries(128);
            entries.insert(entries.end(), last_entries.begin(), last_entries.end());
        }
        std::sort(entries.begin(), entries.end(), [](const logger::Entry* lhs, const logger::Entry* rhs) {
            return lhs->index < rhs->index;
        });

        for (auto& entry : entries) {
            ImVec4 level_colour = {1, 1, 1, 1};
            switch (entry->level) {
                case logger::Level::DEBUG:
                    level_colour = {204.f, 255.f, 51.f, 255.f};
                    break;
                case logger::Level::ERROR:
                    level_colour = {255.f, 51.f, 102.f, 255.f};
                    break;
                case logger::Level::WARNING:
                    level_colour = {255.f, 204.f, 85.f, 255.f};
                    break;
                case logger::Level::INFO:
                    level_colour = {68.f, 170.f, 238.f, 255.f};
                    break;
            }
            level_colour.x /= 255.f;
            level_colour.y /= 255.f;
            level_colour.z /= 255.f;
            level_colour.w /= 255.f;
            ImGui::Text("[%s]", entry->owner.c_str());
            ImGui::SameLine();
            ImGui::TextColored(level_colour, logger::level_to_string(entry->level).data());
            ImGui::SameLine();
            ImGui::Text(entry->message.c_str());
        }
    }
    ImGui::End();
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
    }
}

auto Engine::run() -> void {
    startup();
    main_loop();
    shutdown();
}
auto ENGINE_NS::Engine::main_loop() -> void {
    SDL_Event event;

    const auto tick_rate   = std::chrono::milliseconds(2);
    const auto update_rate = 1.0 / 40.0;

    double accumulator     = 0.0;
    auto last_update       = std::chrono::high_resolution_clock::now();
    auto last_frame        = std::chrono::high_resolution_clock::now();

    while (running_) {
        ++frame_count_;
        FrameMarkStart(StaticNames::EngineLoop);

        auto frame_start = std::chrono::high_resolution_clock::now();
        auto delta       = frame_start - last_update;
        accumulator += delta.count() * 1e-9;
        last_update     = frame_start;

        auto imgui_lock = graphics_.imgui.write();
        auto& imgui     = imgui_lock.get();
        auto& io        = ImGui::GetIO();
        // bool should_discard_mouse    = io.WantCaptureMouse;
        // bool should_discard_keyboard = io.WantCaptureKeyboard;
        while (SDL_PollEvent(&event) != 0) {
            switch (event.type) {
                case SDL_EVENT_QUIT:
                    this->quit();
                    break;
                default:
                    break;
            }
            imgui.events.push_back(event);
        }
        imgui.start_frame();

        this->state_manager.start_frame(this->graphics_);

        this->update();
        while (accumulator > 0.0) {
            FrameMarkStart(StaticNames::FixedUpdate);
            accumulator -= update_rate;
            this->fixed_update(update_rate);
            FrameMarkEnd(StaticNames::FixedUpdate);
        }
        logger.imgui();
        imgui_lock.drop();

        this->state_manager.end_frame();

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
    if (running_) {
        return;
    }
    auto my_logger = logger.get(LogNamespaces::CORE);
    my_logger.get().info("Starting");

    linalg::load_library();
    linalg::load_vector_functions(linalg::g_VECTOR_LIBRARY->library);
    linalg::load_matrix_functions(linalg::g_VECTOR_LIBRARY->library);

    graphics_.initialise();

    my_logger.get().info("Engine ready");
    running_ = true;
}

auto Engine::shutdown() -> void {
    ZoneScoped;
    auto my_logger = logger.get(LogNamespaces::CORE);
    my_logger.get().info("Shutdown");

    state_manager.shutdown(graphics_);
    graphics_.cleanup();

    linalg::g_VECTOR_LIBRARY->~Library();
    running_ = false;
}

auto ENGINE_NS::Engine::update() -> void {
    ZoneScoped;
    this->state_manager.pre_update();
    this->state_manager.update();
    this->state_manager.post_update();
}

auto ENGINE_NS::Engine::fixed_update(double delta_time) -> void {
    ZoneScoped;
    this->state_manager.update_fixed(delta_time);
}

Engine* engine::g_ENGINE = nullptr;

