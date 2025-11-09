#include "engine/linalg/matrix.h"
#include "engine/engine.h"
#include "engine/engine_utils.h"
#include "engine/meta_defines.h"
#include <Tracy/Tracy.hpp>
#include <Windows.h>
#include <libloaderapi.h>

#define VERIFY(function)                                                                                                                   \
    if (!function) {                                                                                                                       \
        engine::crash(ErrorCode::CANNOT_LOAD_LINEAR_ALGEBRA_FUNCTION, __LINE__, __func__, __FILE__);                                       \
    }

void ENGINE_NS::linalg::load_matrix_functions(const void* clibrary) {
    ZoneScoped;
    auto& logger = engine::g_ENGINE->logger.get(engine::LogNamespaces::CORE);
    logger.info("Linking matrix functions");
    HMODULE library = static_cast<HMODULE>(const_cast<void*>(clibrary));
    // Matrix2
    blas2::matrix2::float32::matrix_vector_product =
        reinterpret_cast<blas2::matrix2::float32::matrix_vector_product_def>(GetProcAddress(library, "smvprod2"));
    VERIFY(blas2::matrix2::float32::matrix_vector_product);
    blas2::matrix2::float32::solve_lower_triangular =
        reinterpret_cast<blas2::matrix2::float32::solve_lower_triangular_def>(GetProcAddress(library, "ssolve2l"));
    VERIFY(blas2::matrix2::float32::solve_lower_triangular);
    blas2::matrix2::float32::solve_upper_triangular =
        reinterpret_cast<blas2::matrix2::float32::solve_upper_triangular_def>(GetProcAddress(library, "ssolve2u"));
    VERIFY(blas2::matrix2::float32::solve_upper_triangular);
    blas2::matrix2::float32::solve = reinterpret_cast<blas2::matrix2::float32::solve_def>(GetProcAddress(library, "ssolve2"));
    VERIFY(blas2::matrix2::float32::solve);

    blas2::matrix2::float64::matrix_vector_product =
        reinterpret_cast<blas2::matrix2::float64::matrix_vector_product_def>(GetProcAddress(library, "dmvprod2"));
    VERIFY(blas2::matrix2::float64::matrix_vector_product);
    blas2::matrix2::float64::solve_lower_triangular =
        reinterpret_cast<blas2::matrix2::float64::solve_lower_triangular_def>(GetProcAddress(library, "dsolve2l"));
    VERIFY(blas2::matrix2::float64::solve_lower_triangular);
    blas2::matrix2::float64::solve_upper_triangular =
        reinterpret_cast<blas2::matrix2::float64::solve_upper_triangular_def>(GetProcAddress(library, "dsolve2u"));
    VERIFY(blas2::matrix2::float64::solve_upper_triangular);
    blas2::matrix2::float64::solve = reinterpret_cast<blas2::matrix2::float64::solve_def>(GetProcAddress(library, "dsolve2"));
    VERIFY(blas2::matrix2::float64::solve);
    // Matrix3
    blas2::matrix3::float32::matrix_vector_product =
        reinterpret_cast<blas2::matrix3::float32::matrix_vector_product_def>(GetProcAddress(library, "smvprod3"));
    VERIFY(blas2::matrix3::float32::matrix_vector_product);
    blas2::matrix3::float32::solve_lower_triangular =
        reinterpret_cast<blas2::matrix3::float32::solve_lower_triangular_def>(GetProcAddress(library, "ssolve3l"));
    VERIFY(blas2::matrix3::float32::solve_lower_triangular);
    blas2::matrix3::float32::solve_upper_triangular =
        reinterpret_cast<blas2::matrix3::float32::solve_upper_triangular_def>(GetProcAddress(library, "ssolve3u"));
    VERIFY(blas2::matrix3::float32::solve_upper_triangular);
    blas2::matrix3::float32::solve = reinterpret_cast<blas2::matrix3::float32::solve_def>(GetProcAddress(library, "ssolve3"));
    VERIFY(blas2::matrix3::float32::solve);

    blas2::matrix3::float64::matrix_vector_product =
        reinterpret_cast<blas2::matrix3::float64::matrix_vector_product_def>(GetProcAddress(library, "dmvprod3"));
    VERIFY(blas2::matrix3::float64::matrix_vector_product);
    blas2::matrix3::float64::solve_lower_triangular =
        reinterpret_cast<blas2::matrix3::float64::solve_lower_triangular_def>(GetProcAddress(library, "dsolve3l"));
    VERIFY(blas2::matrix3::float64::solve_lower_triangular);
    blas2::matrix3::float64::solve_upper_triangular =
        reinterpret_cast<blas2::matrix3::float64::solve_upper_triangular_def>(GetProcAddress(library, "dsolve3u"));
    VERIFY(blas2::matrix3::float64::solve_upper_triangular);
    blas2::matrix3::float64::solve = reinterpret_cast<blas2::matrix3::float64::solve_def>(GetProcAddress(library, "dsolve3"));
    VERIFY(blas2::matrix3::float64::solve);
    // matrix4
    blas2::matrix4::float32::matrix_vector_product =
        reinterpret_cast<blas2::matrix4::float32::matrix_vector_product_def>(GetProcAddress(library, "smvprod4"));
    VERIFY(blas2::matrix4::float32::matrix_vector_product);
    blas2::matrix4::float32::solve_lower_triangular =
        reinterpret_cast<blas2::matrix4::float32::solve_lower_triangular_def>(GetProcAddress(library, "ssolve4l"));
    VERIFY(blas2::matrix4::float32::solve_lower_triangular);
    blas2::matrix4::float32::solve_upper_triangular =
        reinterpret_cast<blas2::matrix4::float32::solve_upper_triangular_def>(GetProcAddress(library, "ssolve4u"));
    VERIFY(blas2::matrix4::float32::solve_upper_triangular);
    blas2::matrix4::float32::solve = reinterpret_cast<blas2::matrix4::float32::solve_def>(GetProcAddress(library, "ssolve4"));
    VERIFY(blas2::matrix4::float32::solve);

    blas2::matrix4::float64::matrix_vector_product =
        reinterpret_cast<blas2::matrix4::float64::matrix_vector_product_def>(GetProcAddress(library, "dmvprod4"));
    VERIFY(blas2::matrix4::float64::matrix_vector_product);
    blas2::matrix4::float64::solve_lower_triangular =
        reinterpret_cast<blas2::matrix4::float64::solve_lower_triangular_def>(GetProcAddress(library, "dsolve4l"));
    VERIFY(blas2::matrix4::float64::solve_lower_triangular);
    blas2::matrix4::float64::solve_upper_triangular =
        reinterpret_cast<blas2::matrix4::float64::solve_upper_triangular_def>(GetProcAddress(library, "dsolve4u"));
    VERIFY(blas2::matrix4::float64::solve_upper_triangular);
    blas2::matrix4::float64::solve = reinterpret_cast<blas2::matrix4::float64::solve_def>(GetProcAddress(library, "dsolve4"));
    VERIFY(blas2::matrix4::float64::solve);
    logger.info("Successfully linked matrix functions");
}

namespace ENGINE_NS {
    namespace linalg {
        namespace blas2 {
            namespace matrix2 {
                namespace float32 {
                    matrix_vector_product_def matrix_vector_product   = nullptr;
                    solve_lower_triangular_def solve_lower_triangular = nullptr;
                    solve_upper_triangular_def solve_upper_triangular = nullptr;
                    solve_def solve                                   = nullptr;
                } // namespace float32

                namespace float64 {
                    matrix_vector_product_def matrix_vector_product   = nullptr;
                    solve_lower_triangular_def solve_lower_triangular = nullptr;
                    solve_upper_triangular_def solve_upper_triangular = nullptr;
                    solve_def solve                                   = nullptr;
                } // namespace float64
            } // namespace matrix2
            namespace matrix3 {
                namespace float32 {
                    matrix_vector_product_def matrix_vector_product   = nullptr;
                    solve_lower_triangular_def solve_lower_triangular = nullptr;
                    solve_upper_triangular_def solve_upper_triangular = nullptr;
                    solve_def solve                                   = nullptr;
                } // namespace float32

                namespace float64 {
                    matrix_vector_product_def matrix_vector_product   = nullptr;
                    solve_lower_triangular_def solve_lower_triangular = nullptr;
                    solve_upper_triangular_def solve_upper_triangular = nullptr;
                    solve_def solve                                   = nullptr;
                } // namespace float64
            } // namespace matrix3
            namespace matrix4 {
                namespace float32 {
                    matrix_vector_product_def matrix_vector_product   = nullptr;
                    solve_lower_triangular_def solve_lower_triangular = nullptr;
                    solve_upper_triangular_def solve_upper_triangular = nullptr;
                    solve_def solve                                   = nullptr;
                } // namespace float32

                namespace float64 {
                    matrix_vector_product_def matrix_vector_product   = nullptr;
                    solve_lower_triangular_def solve_lower_triangular = nullptr;
                    solve_upper_triangular_def solve_upper_triangular = nullptr;
                    solve_def solve                                   = nullptr;
                } // namespace float64
            } // namespace matrix4
        } // namespace blas2
    } // namespace linalg
} // namespace ENGINE_NS
