#pragma once

#ifdef INCLUDE_STB_IMAGE_WRITE_H
    #error "Don't include <stb_image_write.h>"
#endif

#if defined(__GNUC__) // GCC, Clang or MinGW
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif

#include <stb_image_write.h>

#if defined(__GNUC__) // GCC, Clang or MinGW
    #pragma GCC diagnostic pop
#endif
