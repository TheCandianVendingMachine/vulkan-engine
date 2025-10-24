#include "engine/linalg/linalg.h"
#include "engine/engine.h"
#include <Tracy/tracy.hpp>
#include <Windows.h>
#include <libloaderapi.h>
#include <memory>

#ifdef _WIN32
    /// MSVC CPUID
    #include <intrin.h>
    #define cpuid(info, x) __cpuidex(info, x, 0)
#else
//  GCC Intrinsics
    #include <cpuid.h>
    /// gcc / clang CPUID
    #define cpuid(info, x) __cpuid_count(x, 0, info[0], info[1], info[2], info[3])
#endif

std::unique_ptr<ENGINE_NS::linalg::Library> ENGINE_NS::linalg::g_VECTOR_LIBRARY = nullptr;

void ENGINE_NS::linalg::load_library() {
    ZoneScoped;
    auto logger = Engine::instance().logger.get(LogNamespaces::CORE);
    logger.info("Initialising linalg library");

    int info[4] = {};
    cpuid(info, 1);


    bool has_avx = (info[2] & (1 << 28)) != 0;
    bool has_sse = (info[2] & (1 << 20)) != 0;

    auto arch    = Arch::SCALAR;
    if (has_sse) {
        logger.info("Detected SSE4.2");
        arch = Arch::SSE;
    } else {
        logger.info("SSE4.2 not present");
    }

    if (has_avx) {
        logger.info("Detected AVX2");
        arch = Arch::AVX;
    } else {
        logger.info("AVX2 not present");
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
    if (!library_) {
        crash(ErrorCode::CANNOT_LOAD_LINEAR_ALGEBRA_LIBRARY);
    }
    library = library_;
    logger.info("Successfully loaded linear algebra library");
}

ENGINE_NS::linalg::Library::~Library() {
    ZoneScoped;
    if (library_) {
        Engine::instance().logger.get(LogNamespaces::CORE).info("Releasing linear algebra library");
        FreeLibrary(static_cast<HMODULE>(library_));
        library_ = nullptr;
    }
}

void ENGINE_NS::linalg::Library::load_scalar_() {
    ZoneScoped;
    auto logger = Engine::instance().logger.get(LogNamespaces::CORE);
    logger.info("Loading scalar linear algebra library");
    library_ = LoadLibraryA("linalg_scalar.dll");
    if (!library_) {
        logger.error("Failed to load linear algebra library");
    }
}

void ENGINE_NS::linalg::Library::load_sse_() {
    ZoneScoped;
    auto logger = Engine::instance().logger.get(LogNamespaces::CORE);
    logger.info("Loading SSE linear algebra library");
    library_ = LoadLibraryA("linalg_sse.dll");
    if (!library_) {
        logger.warning("Failed to load SSE linear algebra library");
        load_scalar_();
    }
}

void ENGINE_NS::linalg::Library::load_avx_() {
    ZoneScoped;
    auto logger = Engine::instance().logger.get(LogNamespaces::CORE);
    logger.info("Loading AVX linear algebra library");
    library_ = LoadLibraryA("linalg_avx.dll");
    if (!library_) {
        logger.warning("Failed to load AVX linear algebra library");
        load_sse_();
    }
}

