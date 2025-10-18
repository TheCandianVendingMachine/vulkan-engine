#pragma once

#define STR(arg) #arg

#define ENGINE_NAME engine
#define ENGINE_NAME_STR STR(ENGINE_NAME)

#define ENGINE_NS ENGINE_NAME

#ifdef IS_DYNAMIC
    #ifdef IS_ENGINE
        #define ENGINE_API __declspec(dllexport)
    #else
        #define ENGINE_API __declspec(dllimport)
    #endif
#else
    #define ENGINE_API
#endif
