#include "engine/linalg/linalg.h"
#include "engine/engine.h"
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
        // arch = Arch::AVX;
    } else {
        logger.info("AVX2 not present");
    }

    g_VECTOR_LIBRARY = std::make_unique<Library>(arch);
}

ENGINE_NS::linalg::Library::Library(Arch arch) : arch_(arch) {
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
    logger.info("Successfully loaded vector library");
}

ENGINE_NS::linalg::Library::~Library() {
    if (library_) {
        Engine::instance().logger.get(LogNamespaces::CORE).info("Releasing vector library");
        FreeLibrary(static_cast<HMODULE>(library_));
        library_ = nullptr;
    }
}

void ENGINE_NS::linalg::Library::load_scalar_() {
    auto logger = Engine::instance().logger.get(LogNamespaces::CORE);
    logger.info("Loading scalar vector library");
    library_ = LoadLibraryA("linalg_scalar.dll");
    if (!library_) {
        logger.error("Failed to load library");
    }
}

void ENGINE_NS::linalg::Library::load_sse_() {
    auto logger = Engine::instance().logger.get(LogNamespaces::CORE);
    logger.info("Loading SSE vector library");
    library_ = LoadLibraryA("linalg_sse.dll");
    if (!library_) {
        logger.warning("Failed to load SSE vector library");
        load_scalar_();
    }
}

void ENGINE_NS::linalg::Library::load_avx_() {
    auto logger = Engine::instance().logger.get(LogNamespaces::CORE);
    logger.info("Loading AVX vector library");
    library_ = LoadLibraryA("linalg_avx.dll");
    if (!library_) {
        logger.warning("Failed to load AVX vector library");
        load_sse_();
    }
}

