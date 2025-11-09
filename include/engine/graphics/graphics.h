#pragma once
#include "engine/graphics/vulkan.h"
#include "engine/meta_defines.h"
#include "linalg/vector.h"
#include <atomic>
#include <chrono>
#include <cstdint>
#include <thread>

struct SDL_Window;
namespace ENGINE_NS {
    class GraphicsEngine {
        public:
            auto initialise() -> void;
            auto draw() -> void;
            auto cleanup() -> void;

        private:
            bool initialised_ = false;

            std::thread render_thread_;
            std::chrono::milliseconds update_rate_;
            std::atomic<bool> running_;

            std::uint64_t frame_number_ = 0;
            ::linalg::Vector2<int> window_extent_{1'700, 900};

            SDL_Window* window_ = nullptr;
            friend class Engine;

            VulkanInstance vulkan_instance_;

            auto init_vulkan_() -> void;
            auto draw_() -> void;
    };
} // namespace ENGINE_NS
