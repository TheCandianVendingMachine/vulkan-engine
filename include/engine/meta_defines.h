#pragma once

#define STR(arg)        #arg

#define ENGINE_NAME     engine
#define ENGINE_NAME_STR STR(ENGINE_NAME)

#define ENGINE_NS       ENGINE_NAME

#ifdef IS_DYNAMIC
    #ifdef IS_ENGINE
        #define ENGINE_API __declspec(dllexport)
    #else
        #define ENGINE_API __declspec(dllimport)
    #endif
#else
    #define ENGINE_API
#endif

enum class ErrorCode : int {
    UNKNOWN = 0,
    CANNOT_LOAD_LINEAR_ALGEBRA_LIBRARY,
    CANNOT_LOAD_LINEAR_ALGEBRA_FUNCTION,
    COUNT
};

static constexpr const char* g_ERROR_CODE_STR[static_cast<size_t>(ErrorCode::COUNT)] = {"Unknown", "Failed to load linear algebra library",
                                                                                        "Failed to load linear algebra functions"};

namespace StaticNames {
    static constexpr const char* EngineLoop     = "Engine loop";
    static constexpr const char* GraphicsInit   = "Graphics Init";
    static constexpr const char* GraphicsDeinit = "Graphics Deinit";
    static constexpr const char* GraphicsDraw   = "Graphics Draw";
} // namespace StaticNames
