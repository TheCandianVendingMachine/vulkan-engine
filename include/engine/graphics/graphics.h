#pragma once
#include "engine/meta_defines.h"
#include "linalg/vector.h"
#include <cstdint>

struct SDL_Window;
namespace ENGINE_NS {
    class GraphicsEngine {
        public:
            void initialise();
            void draw();
            void cleanup();

        private:
            bool initialised_           = false;

            std::uint64_t frame_number_ = 0;
            ::linalg::Vector2<int> window_extent_{1'700, 900};

            SDL_Window* window_ = nullptr;
            friend class Engine;
    };
} // namespace ENGINE_NS
