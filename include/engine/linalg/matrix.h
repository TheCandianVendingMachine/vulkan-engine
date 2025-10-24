#pragma once
#include "engine/meta_defines.h"
#include "linalg/vector.h"

namespace ENGINE_NS {
    namespace linalg {
        void load_matrix_functions(const void* library);
        namespace blas2 {
            namespace matrix2 {
                namespace float32 {
                    typedef float*(__cdecl* matrix_vector_product_def)(float*, float*, const float*);
                    typedef float*(__cdecl* solve_lower_triangular_def)(float*, float*, float*);
                    typedef void(__cdecl* solve_upper_triangular_def)(float*, float*, float*);
                    typedef void(__cdecl* solve_def)(float*, float*, float*);

                    extern matrix_vector_product_def matrix_vector_product;
                    extern solve_lower_triangular_def solve_lower_triangular;
                    extern solve_upper_triangular_def solve_upper_triangular;
                    extern solve_def solve;
                } // namespace float32

                namespace float64 {
                    typedef double*(__cdecl* matrix_vector_product_def)(double*, double*, const double*);
                    typedef double*(__cdecl* solve_lower_triangular_def)(double*, double*, double*);
                    typedef void(__cdecl* solve_upper_triangular_def)(double*, double*, double*);
                    typedef void(__cdecl* solve_def)(double*, double*, double*);

                    extern matrix_vector_product_def matrix_vector_product;
                    extern solve_lower_triangular_def solve_lower_triangular;
                    extern solve_upper_triangular_def solve_upper_triangular;
                    extern solve_def solve;
                } // namespace float64
            } // namespace matrix2
            namespace matrix3 {
                namespace float32 {
                    typedef float*(__cdecl* matrix_vector_product_def)(float*, float*, const float*);
                    typedef float*(__cdecl* solve_lower_triangular_def)(float*, float*, float*);
                    typedef void(__cdecl* solve_upper_triangular_def)(float*, float*, float*);
                    typedef void(__cdecl* solve_def)(float*, float*, float*);

                    extern matrix_vector_product_def matrix_vector_product;
                    extern solve_lower_triangular_def solve_lower_triangular;
                    extern solve_upper_triangular_def solve_upper_triangular;
                    extern solve_def solve;
                } // namespace float32

                namespace float64 {
                    typedef double*(__cdecl* matrix_vector_product_def)(double*, double*, const double*);
                    typedef double*(__cdecl* solve_lower_triangular_def)(double*, double*, double*);
                    typedef void(__cdecl* solve_upper_triangular_def)(double*, double*, double*);
                    typedef void(__cdecl* solve_def)(double*, double*, double*);

                    extern matrix_vector_product_def matrix_vector_product;
                    extern solve_lower_triangular_def solve_lower_triangular;
                    extern solve_upper_triangular_def solve_upper_triangular;
                    extern solve_def solve;
                } // namespace float64
            } // namespace matrix3
            namespace matrix4 {
                namespace float32 {
                    typedef float*(__cdecl* matrix_vector_product_def)(float*, float*, const float*);
                    typedef float*(__cdecl* solve_lower_triangular_def)(float*, float*, float*);
                    typedef void(__cdecl* solve_upper_triangular_def)(float*, float*, float*);
                    typedef void(__cdecl* solve_def)(float*, float*, float*);

                    extern matrix_vector_product_def matrix_vector_product;
                    extern solve_lower_triangular_def solve_lower_triangular;
                    extern solve_upper_triangular_def solve_upper_triangular;
                    extern solve_def solve;
                } // namespace float32

                namespace float64 {
                    typedef double*(__cdecl* matrix_vector_product_def)(double*, double*, const double*);
                    typedef double*(__cdecl* solve_lower_triangular_def)(double*, double*, double*);
                    typedef void(__cdecl* solve_upper_triangular_def)(double*, double*, double*);
                    typedef void(__cdecl* solve_def)(double*, double*, double*);

                    extern matrix_vector_product_def matrix_vector_product;
                    extern solve_lower_triangular_def solve_lower_triangular;
                    extern solve_upper_triangular_def solve_upper_triangular;
                    extern solve_def solve;
                } // namespace float64
            } // namespace matrix4
        } // namespace blas2
    } // namespace linalg
} // namespace ENGINE_NS
