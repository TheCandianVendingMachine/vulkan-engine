#include "engine/linalg/linalg.h"

#include "engine/engine.h"
#include "engine/engine_utils.h"
#include "engine/shared_library.h"

#include <tracy/Tracy.hpp>
#include <memory>

#ifdef _WIN32
    /// MSVC CPUID
    #include <intrin.h>
    #define cpuid(info, x) __cpuidex(info, x, 0)

    constexpr const char* AVX_LIBRARY_NAME = "linalg_avx.dll";
    constexpr const char* SSE_LIBRARY_NAME = "linalg_sse.dll";
    constexpr const char* SCALAR_LIBRARY_NAME = "linalg_scalar.dll";
#else
//  GCC Intrinsics
    #include <cpuid.h>
    /// gcc / clang CPUID
    #define cpuid(info, x) __cpuid_count(x, 0, info[0], info[1], info[2], info[3])

    constexpr const char* AVX_LIBRARY_NAME = "linalg_avx.so";
    constexpr const char* SSE_LIBRARY_NAME = "linalg_sse.so";
    constexpr const char* SCALAR_LIBRARY_NAME = "linalg_scalar.so";
#endif

std::unique_ptr<ENGINE_NS::linalg::Library> ENGINE_NS::linalg::g_VECTOR_LIBRARY = nullptr;

void ENGINE_NS::linalg::load_library() {
    ZoneScoped;
    auto logger = Engine::instance().logger.get(LogNamespaces::CORE);
    logger.get().info("Initialising linalg library");

    int info[4] = {};
    cpuid(info, 1);


    bool has_avx = (info[2] & (1 << 28)) != 0;
    bool has_sse = (info[2] & (1 << 20)) != 0;

    auto arch = Arch::SCALAR;
    if (has_sse) {
        logger.get().info("Detected SSE4.2");
        // arch = Arch::SSE;
    } else {
        logger.get().info("SSE4.2 not present");
    }

    if (has_avx) {
        logger.get().info("Detected AVX2");
        // arch = Arch::AVX;
    } else {
        logger.get().info("AVX2 not present");
    }

    g_VECTOR_LIBRARY = std::make_unique<Library>(arch);
}

ENGINE_NS::linalg::Library::Library(Arch arch) : arch_(arch) {
    ZoneScoped;
    auto logger = Engine::instance().logger.get(LogNamespaces::CORE);
    switch (arch) {
        case Arch::SCALAR:
            load_scalar_();
            break;
        case Arch::SSE:
            load_sse_();
            break;
        case Arch::AVX:
            load_avx_();
            break;
    }
    if (library_ == nullptr) {
        crash(ErrorCode::CANNOT_LOAD_LINEAR_ALGEBRA_LIBRARY);
    }
    const_cast<ENGINE_NS::LibraryHandle&>(library) = library_;
    logger.get().info("Successfully loaded linear algebra library");
}

ENGINE_NS::linalg::Library::~Library() {
    ZoneScoped;
    if (library_ != nullptr) {
        auto logger = Engine::instance().logger.get(LogNamespaces::CORE);
        logger.get().info("Releasing linear algebra library");
        ENGINE_NS::unload_library(library_);
        library_ = nullptr;
    }
}

void ENGINE_NS::linalg::Library::load_scalar_() {
    ZoneScoped;
    auto logger = Engine::instance().logger.get(LogNamespaces::CORE);
    logger.get().info("Loading scalar linear algebra library");
    library_ = ENGINE_NS::load_library(SCALAR_LIBRARY_NAME);
    if (library_ == nullptr) {
        logger.get().error("Failed to load linear algebra library");
    }
}

void ENGINE_NS::linalg::Library::load_sse_() {
    ZoneScoped;
    auto logger = Engine::instance().logger.get(LogNamespaces::CORE);
    logger.get().info("Loading SSE linear algebra library");
    library_ = ENGINE_NS::load_library(SSE_LIBRARY_NAME);
    if (library_ == nullptr) {
        logger.get().warning("Failed to load SSE linear algebra library");
        load_scalar_();
    }
}

void ENGINE_NS::linalg::Library::load_avx_() {
    ZoneScoped;
    auto logger = Engine::instance().logger.get(LogNamespaces::CORE);
    logger.get().info("Loading AVX linear algebra library");
    library_ = ENGINE_NS::load_library(AVX_LIBRARY_NAME);
    if (library_ == nullptr) {
        logger.get().warning("Failed to load AVX linear algebra library");
        load_sse_();
    }
}

