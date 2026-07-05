#pragma once
#include "engine/meta_defines.h"
#include "engine/shared_library.h"

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

                const Arch& arch = arch_;

                const SharedLibrary& library = library_;

            private:
                void load_scalar_();
                void load_sse_();
                void load_avx_();

                Arch arch_;
                SharedLibrary library_;
        };

        extern std::unique_ptr<Library> g_VECTOR_LIBRARY;
        void load_library();
    } // namespace linalg
} // namespace ENGINE_NS
