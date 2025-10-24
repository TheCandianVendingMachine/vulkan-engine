#pragma once
#include "engine/meta_defines.h"
#include "linalg/vector.h"

namespace ENGINE_NS {
    namespace linalg {
        void load_vector_functions(const void* library);
        namespace blas1 {
            namespace vector2 {
                namespace float32 {
                    typedef float*(__cdecl* axpy_def)(float*, const float, const float*, const float*);
                    typedef float*(__cdecl* scale_def)(float*, const float, const float*);
                    typedef void(__cdecl* copy_def)(float*, const float*);
                    typedef void(__cdecl* swap_def)(float*, float*);
                    typedef float(__cdecl* dot_def)(const float*, const float*);
                    typedef float(__cdecl* component_sum_def)(const float*);
                    typedef float(__cdecl* magnitude_def)(const float*);
                    typedef float(__cdecl* component_max_def)(const float*);

                    extern axpy_def axpy;
                    extern scale_def scale;
                    extern copy_def copy;
                    extern swap_def swap;
                    extern dot_def dot;
                    extern component_sum_def component_sum;
                    extern magnitude_def magnitude;
                    extern component_max_def component_max;
                } // namespace float32

                namespace float64 {
                    typedef double*(__cdecl* axpy_def)(double*, const double, const double*, const double*);
                    typedef double*(__cdecl* scale_def)(double*, const double, const double*);
                    typedef void(__cdecl* copy_def)(double*, const double*);
                    typedef void(__cdecl* swap_def)(double*, double*);
                    typedef double(__cdecl* dot_def)(const double*, const double*);
                    typedef double(__cdecl* component_sum_def)(const double*);
                    typedef double(__cdecl* magnitude_def)(const double*);
                    typedef double(__cdecl* component_max_def)(const double*);

                    extern axpy_def axpy;
                    extern scale_def scale;
                    extern copy_def copy;
                    extern swap_def swap;
                    extern dot_def dot;
                    extern component_sum_def component_sum;
                    extern magnitude_def magnitude;
                    extern component_max_def component_max;
                } // namespace float64
            } // namespace vector2
            namespace vector3 {
                namespace float32 {
                    typedef float*(__cdecl* axpy_def)(float*, const float, const float*, const float*);
                    typedef float*(__cdecl* scale_def)(float*, const float, const float*);
                    typedef void(__cdecl* copy_def)(float*, const float*);
                    typedef void(__cdecl* swap_def)(float*, float*);
                    typedef float(__cdecl* dot_def)(const float*, const float*);
                    typedef float(__cdecl* component_sum_def)(const float*);
                    typedef float(__cdecl* magnitude_def)(const float*);
                    typedef float(__cdecl* component_max_def)(const float*);

                    extern axpy_def axpy;
                    extern scale_def scale;
                    extern copy_def copy;
                    extern swap_def swap;
                    extern dot_def dot;
                    extern component_sum_def component_sum;
                    extern magnitude_def magnitude;
                    extern component_max_def component_max;
                } // namespace float32

                namespace float64 {
                    typedef double*(__cdecl* axpy_def)(double*, const double, const double*, const double*);
                    typedef double*(__cdecl* scale_def)(double*, const double, const double*);
                    typedef void(__cdecl* copy_def)(double*, const double*);
                    typedef void(__cdecl* swap_def)(double*, double*);
                    typedef double(__cdecl* dot_def)(const double*, const double*);
                    typedef double(__cdecl* component_sum_def)(const double*);
                    typedef double(__cdecl* magnitude_def)(const double*);
                    typedef double(__cdecl* component_max_def)(const double*);

                    extern axpy_def axpy;
                    extern scale_def scale;
                    extern copy_def copy;
                    extern swap_def swap;
                    extern dot_def dot;
                    extern component_sum_def component_sum;
                    extern magnitude_def magnitude;
                    extern component_max_def component_max;
                } // namespace float64
            } // namespace vector3
            namespace vector4 {
                namespace float32 {
                    typedef float*(__cdecl* axpy_def)(float*, const float, const float*, const float*);
                    typedef float*(__cdecl* scale_def)(float*, const float, const float*);
                    typedef void(__cdecl* copy_def)(float*, const float*);
                    typedef void(__cdecl* swap_def)(float*, float*);
                    typedef float(__cdecl* dot_def)(const float*, const float*);
                    typedef float(__cdecl* component_sum_def)(const float*);
                    typedef float(__cdecl* magnitude_def)(const float*);
                    typedef float(__cdecl* component_max_def)(const float*);

                    extern axpy_def axpy;
                    extern scale_def scale;
                    extern copy_def copy;
                    extern swap_def swap;
                    extern dot_def dot;
                    extern component_sum_def component_sum;
                    extern magnitude_def magnitude;
                    extern component_max_def component_max;
                } // namespace float32

                namespace float64 {
                    typedef double*(__cdecl* axpy_def)(double*, const double, const double*, const double*);
                    typedef double*(__cdecl* scale_def)(double*, const double, const double*);
                    typedef void(__cdecl* copy_def)(double*, const double*);
                    typedef void(__cdecl* swap_def)(double*, double*);
                    typedef double(__cdecl* dot_def)(const double*, const double*);
                    typedef double(__cdecl* component_sum_def)(const double*);
                    typedef double(__cdecl* magnitude_def)(const double*);
                    typedef double(__cdecl* component_max_def)(const double*);

                    extern axpy_def axpy;
                    extern scale_def scale;
                    extern copy_def copy;
                    extern swap_def swap;
                    extern dot_def dot;
                    extern component_sum_def component_sum;
                    extern magnitude_def magnitude;
                    extern component_max_def component_max;
                } // namespace float64
            } // namespace vector4
        } // namespace blas1
    } // namespace linalg
} // namespace ENGINE_NS
