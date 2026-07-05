#include "engine/engine.h"
#include "engine/linalg/matrix_operations.h"
#include "engine/meta_defines.h"
#include "engine/shared_library.h"

#include <tracy/Tracy.hpp>

void ENGINE_NS::linalg::load_matrix_functions(const void* clibrary) {
    ZoneScoped;
    auto logger = engine::g_ENGINE->logger.get(engine::LogNamespaces::CORE);
    logger.get().info("Linking matrix functions");
    LibraryHandle library               = const_cast<void*>(clibrary);
    // Matrix2
    ENGINE_NS::get_symbol(
        library,
        blas2::matrix2::float32::matrix_vector_product,
        "smvprod2"
    );
    ENGINE_NS::get_symbol(
        library,
        blas2::matrix2::float32::solve_lower_triangular,
        "ssolve2l"
    );
    ENGINE_NS::get_symbol(
        library,
        blas2::matrix2::float32::solve_upper_triangular,
        "ssolve2u"
    );
    ENGINE_NS::get_symbol(
        library,
        blas2::matrix2::float32::solve,
        "ssolve2"
    );

    ENGINE_NS::get_symbol(
        library,
        blas2::matrix2::float64::matrix_vector_product,
        "dmvprod2"
    );
    ENGINE_NS::get_symbol(
        library,
        blas2::matrix2::float64::solve_lower_triangular,
        "dsolve2l"
    );
    ENGINE_NS::get_symbol(
        library,
        blas2::matrix2::float64::solve_upper_triangular,
        "dsolve2u"
    );
    ENGINE_NS::get_symbol(
        library,
        blas2::matrix2::float64::solve,
        "dsolve2"
    );
    // Matrix3
    ENGINE_NS::get_symbol(
        library,
        blas2::matrix3::float32::matrix_vector_product,
        "smvprod3"
    );
    ENGINE_NS::get_symbol(
        library,
        blas2::matrix3::float32::solve_lower_triangular,
        "ssolve3l"
    );
    ENGINE_NS::get_symbol(
        library,
        blas2::matrix3::float32::solve_upper_triangular,
        "ssolve3u"
    );
    ENGINE_NS::get_symbol(
        library,
        blas2::matrix3::float32::solve,
        "ssolve3"
    );

    ENGINE_NS::get_symbol(
        library,
        blas2::matrix3::float64::matrix_vector_product,
        "dmvprod3"
    );
    ENGINE_NS::get_symbol(
        library,
        blas2::matrix3::float64::solve_lower_triangular,
        "dsolve3l"
    );
    ENGINE_NS::get_symbol(
        library,
        blas2::matrix3::float64::solve_upper_triangular,
        "dsolve3u"
    );
    ENGINE_NS::get_symbol(
        library,
        blas2::matrix3::float64::solve,
        "dsolve3"
    );
    // matrix4
    ENGINE_NS::get_symbol(
        library,
        blas2::matrix4::float32::matrix_vector_product,
        "smvprod4"
    );
    ENGINE_NS::get_symbol(
        library,
        blas2::matrix4::float32::solve_lower_triangular,
        "ssolve4l"
    );
    ENGINE_NS::get_symbol(
        library,
        blas2::matrix4::float32::solve_upper_triangular,
        "ssolve4u"
    );
    ENGINE_NS::get_symbol(
        library,
        blas2::matrix4::float32::solve,
        "ssolve4"
    );

    ENGINE_NS::get_symbol(
        library,
        blas2::matrix4::float64::matrix_vector_product,
        "dmvprod4"
    );
    ENGINE_NS::get_symbol(
        library,
        blas2::matrix4::float64::solve_lower_triangular,
        "dsolve4l"
    );
    ENGINE_NS::get_symbol(
        library,
        blas2::matrix4::float64::solve_upper_triangular,
        "dsolve4u"
    );
    ENGINE_NS::get_symbol(
        library,
        blas2::matrix4::float64::solve,
        "dsolve4"
    );
    logger.get().info("Successfully linked matrix functions");
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
