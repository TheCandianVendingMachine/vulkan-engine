#pragma once

#ifdef _WIN32
#ifdef LINALG_BUILD
#define LINALG_API __declspec(dllexport)
#else
#define LINALG_API __declspec(dllimport)
#endif
#else
#define LINALG_API
#endif

