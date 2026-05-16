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
    VULKAN_ERROR,
    CANNOT_READ_FILE,
    COUNT
};

static constexpr const char* g_ERROR_CODE_STR[static_cast<size_t>(ErrorCode::COUNT)] = {
  "Unknown", "Failed to load linear algebra library", "Failed to load linear algebra functions", "Vulkan Error", "Cannot read file"};

namespace StaticNames {
    static constexpr const char* EngineLoop                 = "Engine loop";
    static constexpr const char* FixedUpdate                = "Fixed Update";
    static constexpr const char* GraphicsInit               = "Graphics Init";
    static constexpr const char* GraphicsDeinit             = "Graphics Deinit";
    static constexpr const char* GraphicsDraw               = "Graphics Draw";
    static constexpr const char* GraphicsThreadName         = "Renderer";
    static constexpr const char* UploadThreadName           = "Uploader";
    static constexpr const char* CompileThreadName          = "Pipeline Compiler";
    static constexpr const char* CompileRun                 = "Pipeline Compile";
    static constexpr const char* RenderLoop                 = "Render Loop";
    static constexpr const char* UploadLoop                 = "Upload Loop";
    static constexpr const char* MainCommandBufferName      = "MainCommandBuffer";
    static constexpr const char* ImmediateCommandBufferName = "ImmediateCommandBuffer";
    static constexpr const char* DeleteRegisteredPipelines  = "Delete Registered Pipelines";
    static constexpr const char* PopGameStates              = "Pop Game Stats";
    static constexpr const char* PushGameStates             = "Push Game Stats";
} // namespace StaticNames
