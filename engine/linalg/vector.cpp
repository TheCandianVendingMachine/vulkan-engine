#include "engine/engine.h"
#include "engine/linalg/vector_operations.h"
#include "engine/meta_defines.h"
#include "engine/shared_library.h"

#include <tracy/Tracy.hpp>

namespace ENGINE_NS {
    namespace linalg {
        void load_vector_functions(const void* clibrary) {
            ZoneScoped;
            auto logger = engine::g_ENGINE->logger.get(engine::LogNamespaces::CORE);
            logger.get().info("Linking vector functions");
            LibraryHandle library               = const_cast<void*>(clibrary);
            // Vector2
            load_symbol(
                library,
                blas1::vector2::float32::axpy,
                "saxpy2"
            );
            load_symbol(
                library,
                blas1::vector2::float32::scale,
                "sscale2"
            );
            load_symbol(
                library,
                blas1::vector2::float32::copy,
                "scopy2"
            );
            load_symbol(
                library,
                blas1::vector2::float32::swap,
                "sswap2"
            );
            load_symbol(
                library,
                blas1::vector2::float32::dot,
                "sdot2"
            );
            load_symbol(
                library,
                blas1::vector2::float32::component_sum,
                "scomponentsum2"
            );
            load_symbol(
                library,
                blas1::vector2::float32::magnitude,
                "smagnitude2"
            );
            load_symbol(
                library,
                blas1::vector2::float32::component_max,
                "scomponentsum2"
            );


            load_symbol(
                library,
                blas1::vector2::float64::axpy,
                "daxpy2"
            );
            load_symbol(
                library,
                blas1::vector2::float64::scale,
                "dscale2"
            );
            load_symbol(
                library,
                blas1::vector2::float64::copy,
                "dcopy2"
            );
            load_symbol(
                library,
                blas1::vector2::float64::swap,
                "dswap2"
            );
            load_symbol(
                library,
                blas1::vector2::float64::dot,
                "ddot2"
            );
            load_symbol(
                library,
                blas1::vector2::float64::component_sum,
                "dcomponentsum2"
            );
            load_symbol(
                library,
                blas1::vector2::float64::magnitude,
                "dmagnitude2"
            );
            load_symbol(
                library,
                blas1::vector2::float64::component_max,
                "dcomponentsum2"
            );

            // Vector3
            load_symbol(
                library,
                blas1::vector3::float32::axpy,
                "saxpy3"
            );
            load_symbol(
                library,
                blas1::vector3::float32::scale,
                "sscale3"
            );
            load_symbol(
                library,
                blas1::vector3::float32::copy,
                "scopy3"
            );
            load_symbol(
                library,
                blas1::vector3::float32::swap,
                "sswap3"
            );
            load_symbol(
                library,
                blas1::vector3::float32::dot,
                "sdot3"
            );
            load_symbol(
                library,
                blas1::vector3::float32::component_sum,
                "scomponentsum3"
            );
            load_symbol(
                library,
                blas1::vector3::float32::magnitude,
                "smagnitude3"
            );
            load_symbol(
                library,
                blas1::vector3::float32::component_max,
                "scomponentsum3"
            );


            load_symbol(
                library,
                blas1::vector3::float64::axpy,
                "daxpy3"
            );
            load_symbol(
                library,
                blas1::vector3::float64::scale,
                "dscale3"
            );
            load_symbol(
                library,
                blas1::vector3::float64::copy,
                "dcopy3"
            );
            load_symbol(
                library,
                blas1::vector3::float64::swap,
                "sswap3"
            );
            load_symbol(
                library,
                blas1::vector3::float64::dot,
                "ddot3"
            );
            load_symbol(
                library,
                blas1::vector3::float64::component_sum,
                "dcomponentsum3"
            );
            load_symbol(
                library,
                blas1::vector3::float64::magnitude,
                "dmagnitude3"
            );
            load_symbol(
                library,
                blas1::vector3::float64::component_max,
                "dcomponentsum3"
            );

            // Vector4

            load_symbol(
                library,
                blas1::vector4::float32::axpy,
                "saxpy4"
            );
            load_symbol(
                library,
                blas1::vector4::float32::scale,
                "sscale4"
            );
            load_symbol(
                library,
                blas1::vector4::float32::copy,
                "scopy4"
            );
            load_symbol(
                library,
                blas1::vector4::float32::swap,
                "sswap4"
            );
            load_symbol(
                library,
                blas1::vector4::float32::dot,
                "sdot4"
            );
            load_symbol(
                library,
                blas1::vector4::float32::component_sum,
                "scomponentsum4"
            );
            load_symbol(
                library,
                blas1::vector4::float32::magnitude,
                "smagnitude4"
            );
            load_symbol(
                library,
                blas1::vector4::float32::component_max,
                "scomponentsum4"
            );


            load_symbol(
                library,
                blas1::vector4::float64::axpy,
                "daxpy4"
            );
            load_symbol(
                library,
                blas1::vector4::float64::scale,
                "dscale4"
            );
            load_symbol(
                library,
                blas1::vector4::float64::copy,
                "dcopy4"
            );
            load_symbol(
                library,
                blas1::vector4::float64::swap,
                "sswap4"
            );
            load_symbol(
                library,
                blas1::vector4::float64::dot,
                "ddot4"
            );
            load_symbol(
                library,
                blas1::vector4::float64::component_sum,
                "dcomponentsum4"
            );
            load_symbol(
                library,
                blas1::vector4::float64::magnitude,
                "dmagnitude4"
            );
            load_symbol(
                library,
                blas1::vector4::float64::component_max,
                "dcomponentsum4"
            );
            logger.get().info("Successfully linked vector functions");
        }
    } // namespace linalg
} // namespace ENGINE_NS

namespace ENGINE_NS {
    namespace linalg {
        namespace blas1 {
            namespace vector2 {
                namespace float32 {
                    axpy_def axpy                   = nullptr;
                    scale_def scale                 = nullptr;
                    copy_def copy                   = nullptr;
                    swap_def swap                   = nullptr;
                    dot_def dot                     = nullptr;
                    component_sum_def component_sum = nullptr;
                    magnitude_def magnitude         = nullptr;
                    component_max_def component_max = nullptr;
                } // namespace float32

                namespace float64 {
                    axpy_def axpy                   = nullptr;
                    scale_def scale                 = nullptr;
                    copy_def copy                   = nullptr;
                    swap_def swap                   = nullptr;
                    dot_def dot                     = nullptr;
                    component_sum_def component_sum = nullptr;
                    magnitude_def magnitude         = nullptr;
                    component_max_def component_max = nullptr;
                } // namespace float64
            } // namespace vector2
            namespace vector3 {
                namespace float32 {
                    axpy_def axpy                   = nullptr;
                    scale_def scale                 = nullptr;
                    copy_def copy                   = nullptr;
                    swap_def swap                   = nullptr;
                    dot_def dot                     = nullptr;
                    component_sum_def component_sum = nullptr;
                    magnitude_def magnitude         = nullptr;
                    component_max_def component_max = nullptr;
                } // namespace float32

                namespace float64 {
                    axpy_def axpy                   = nullptr;
                    scale_def scale                 = nullptr;
                    copy_def copy                   = nullptr;
                    swap_def swap                   = nullptr;
                    dot_def dot                     = nullptr;
                    component_sum_def component_sum = nullptr;
                    magnitude_def magnitude         = nullptr;
                    component_max_def component_max = nullptr;
                } // namespace float64
            } // namespace vector3
            namespace vector4 {
                namespace float32 {
                    axpy_def axpy                   = nullptr;
                    scale_def scale                 = nullptr;
                    copy_def copy                   = nullptr;
                    swap_def swap                   = nullptr;
                    dot_def dot                     = nullptr;
                    component_sum_def component_sum = nullptr;
                    magnitude_def magnitude         = nullptr;
                    component_max_def component_max = nullptr;
                } // namespace float32

                namespace float64 {
                    axpy_def axpy                   = nullptr;
                    scale_def scale                 = nullptr;
                    copy_def copy                   = nullptr;
                    swap_def swap                   = nullptr;
                    dot_def dot                     = nullptr;
                    component_sum_def component_sum = nullptr;
                    magnitude_def magnitude         = nullptr;
                    component_max_def component_max = nullptr;
                } // namespace float64
            } // namespace vector4
        } // namespace blas1
    } // namespace linalg
} // namespace ENGINE_NS

