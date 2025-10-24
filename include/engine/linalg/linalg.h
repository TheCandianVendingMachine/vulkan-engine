#pragma once
#include "engine/meta_defines.h"
#include <memory>

namespace ENGINE_NS {
    namespace linalg {
        enum class Arch {
            SCALAR,
            SSE,
            AVX
        };
        class Library {
            public:
                explicit Library(Arch arch);
                ~Library();

                const Arch& arch    = arch_;

                const void* library = nullptr;

            private:
                void load_scalar_();
                void load_sse_();
                void load_avx_();

                Arch arch_;
                void* library_ = nullptr;
        };

        extern std::unique_ptr<Library> g_VECTOR_LIBRARY;
        void load_library();
    } // namespace linalg
} // namespace ENGINE_NS
