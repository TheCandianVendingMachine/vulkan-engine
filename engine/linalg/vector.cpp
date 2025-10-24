#include "engine/linalg/vector.h"
#include "engine/engine.h"
#include "engine/meta_defines.h"
#include <Windows.h>
#include <libloaderapi.h>

#define VERIFY(function)                                                                                                                   \
    if (!function) {                                                                                                                       \
        engine::crash(ErrorCode::CANNOT_LOAD_LINEAR_ALGEBRA_FUNCTION, __LINE__, __func__, __FILE__);                                       \
    }

namespace ENGINE_NS {
    namespace linalg {
        void load_vector_functions(const void* clibrary) {
            auto& logger = engine::g_ENGINE->logger.get(engine::LogNamespaces::CORE);
            logger.info("Linking vector functions");
            HMODULE library               = static_cast<HMODULE>(const_cast<void*>(clibrary));
            // Vector2
            blas1::vector2::float32::axpy = reinterpret_cast<blas1::vector2::float32::axpy_def>(GetProcAddress(library, "saxpy2"));
            VERIFY(blas1::vector2::float32::axpy);
            blas1::vector2::float32::scale = reinterpret_cast<blas1::vector2::float32::scale_def>(GetProcAddress(library, "sscale2"));
            VERIFY(blas1::vector2::float32::scale);
            blas1::vector2::float32::copy = reinterpret_cast<blas1::vector2::float32::copy_def>(GetProcAddress(library, "scopy2"));
            VERIFY(blas1::vector2::float32::copy);
            blas1::vector2::float32::swap = reinterpret_cast<blas1::vector2::float32::swap_def>(GetProcAddress(library, "sswap2"));
            VERIFY(blas1::vector2::float32::swap);
            blas1::vector2::float32::dot = reinterpret_cast<blas1::vector2::float32::dot_def>(GetProcAddress(library, "sdot2"));
            blas1::vector2::float32::component_sum =
                reinterpret_cast<blas1::vector2::float32::component_sum_def>(GetProcAddress(library, "scomponentsum2"));
            VERIFY(blas1::vector2::float32::dot);
            blas1::vector2::float32::magnitude =
                reinterpret_cast<blas1::vector2::float32::magnitude_def>(GetProcAddress(library, "smagnitude2"));
            VERIFY(blas1::vector2::float32::magnitude);
            blas1::vector2::float32::component_max =
                reinterpret_cast<blas1::vector2::float32::component_max_def>(GetProcAddress(library, "scomponentmax2"));
            VERIFY(blas1::vector2::float32::component_max);


            blas1::vector2::float64::axpy = reinterpret_cast<blas1::vector2::float64::axpy_def>(GetProcAddress(library, "daxpy2"));
            VERIFY(blas1::vector2::float64::axpy);
            blas1::vector2::float64::scale = reinterpret_cast<blas1::vector2::float64::scale_def>(GetProcAddress(library, "dscale2"));
            VERIFY(blas1::vector2::float64::scale);
            blas1::vector2::float64::copy = reinterpret_cast<blas1::vector2::float64::copy_def>(GetProcAddress(library, "dcopy2"));
            VERIFY(blas1::vector2::float64::copy);
            blas1::vector2::float64::swap = reinterpret_cast<blas1::vector2::float64::swap_def>(GetProcAddress(library, "dswap2"));
            VERIFY(blas1::vector2::float64::swap);
            blas1::vector2::float64::dot = reinterpret_cast<blas1::vector2::float64::dot_def>(GetProcAddress(library, "ddot2"));
            blas1::vector2::float64::component_sum =
                reinterpret_cast<blas1::vector2::float64::component_sum_def>(GetProcAddress(library, "dcomponentsum2"));
            VERIFY(blas1::vector2::float64::dot);
            blas1::vector2::float64::magnitude =
                reinterpret_cast<blas1::vector2::float64::magnitude_def>(GetProcAddress(library, "dmagnitude2"));
            VERIFY(blas1::vector2::float64::magnitude);
            blas1::vector2::float64::component_max =
                reinterpret_cast<blas1::vector2::float64::component_max_def>(GetProcAddress(library, "dcomponentmax2"));
            VERIFY(blas1::vector2::float64::component_max);

            // Vector3

            blas1::vector3::float32::axpy = reinterpret_cast<blas1::vector3::float32::axpy_def>(GetProcAddress(library, "saxpy3"));
            VERIFY(blas1::vector3::float32::axpy);
            blas1::vector3::float32::scale = reinterpret_cast<blas1::vector3::float32::scale_def>(GetProcAddress(library, "sscale3"));
            VERIFY(blas1::vector3::float32::scale);
            blas1::vector3::float32::copy = reinterpret_cast<blas1::vector3::float32::copy_def>(GetProcAddress(library, "scopy3"));
            VERIFY(blas1::vector3::float32::copy);
            blas1::vector3::float32::swap = reinterpret_cast<blas1::vector3::float32::swap_def>(GetProcAddress(library, "sswap3"));
            VERIFY(blas1::vector3::float32::swap);
            blas1::vector3::float32::dot = reinterpret_cast<blas1::vector3::float32::dot_def>(GetProcAddress(library, "sdot3"));
            blas1::vector3::float32::component_sum =
                reinterpret_cast<blas1::vector3::float32::component_sum_def>(GetProcAddress(library, "scomponentsum3"));
            VERIFY(blas1::vector3::float32::dot);
            blas1::vector3::float32::magnitude =
                reinterpret_cast<blas1::vector3::float32::magnitude_def>(GetProcAddress(library, "smagnitude3"));
            VERIFY(blas1::vector3::float32::magnitude);
            blas1::vector3::float32::component_max =
                reinterpret_cast<blas1::vector3::float32::component_max_def>(GetProcAddress(library, "scomponentmax3"));
            VERIFY(blas1::vector3::float32::component_max);


            blas1::vector3::float64::axpy = reinterpret_cast<blas1::vector3::float64::axpy_def>(GetProcAddress(library, "daxpy3"));
            VERIFY(blas1::vector3::float64::axpy);
            blas1::vector3::float64::scale = reinterpret_cast<blas1::vector3::float64::scale_def>(GetProcAddress(library, "dscale3"));
            VERIFY(blas1::vector3::float64::scale);
            blas1::vector3::float64::copy = reinterpret_cast<blas1::vector3::float64::copy_def>(GetProcAddress(library, "dcopy3"));
            VERIFY(blas1::vector3::float64::copy);
            blas1::vector3::float64::swap = reinterpret_cast<blas1::vector3::float64::swap_def>(GetProcAddress(library, "dswap3"));
            VERIFY(blas1::vector3::float64::swap);
            blas1::vector3::float64::dot = reinterpret_cast<blas1::vector3::float64::dot_def>(GetProcAddress(library, "ddot3"));
            blas1::vector3::float64::component_sum =
                reinterpret_cast<blas1::vector3::float64::component_sum_def>(GetProcAddress(library, "dcomponentsum3"));
            VERIFY(blas1::vector3::float64::dot);
            blas1::vector3::float64::magnitude =
                reinterpret_cast<blas1::vector3::float64::magnitude_def>(GetProcAddress(library, "dmagnitude3"));
            VERIFY(blas1::vector3::float64::magnitude);
            blas1::vector3::float64::component_max =
                reinterpret_cast<blas1::vector3::float64::component_max_def>(GetProcAddress(library, "dcomponentmax3"));
            VERIFY(blas1::vector3::float64::component_max);

            // Vector4

            blas1::vector4::float32::axpy = reinterpret_cast<blas1::vector4::float32::axpy_def>(GetProcAddress(library, "saxpy4"));
            VERIFY(blas1::vector4::float32::axpy);
            blas1::vector4::float32::scale = reinterpret_cast<blas1::vector4::float32::scale_def>(GetProcAddress(library, "sscale4"));
            VERIFY(blas1::vector4::float32::scale);
            blas1::vector4::float32::copy = reinterpret_cast<blas1::vector4::float32::copy_def>(GetProcAddress(library, "scopy4"));
            VERIFY(blas1::vector4::float32::copy);
            blas1::vector4::float32::swap = reinterpret_cast<blas1::vector4::float32::swap_def>(GetProcAddress(library, "sswap4"));
            VERIFY(blas1::vector4::float32::swap);
            blas1::vector4::float32::dot = reinterpret_cast<blas1::vector4::float32::dot_def>(GetProcAddress(library, "sdot4"));
            blas1::vector4::float32::component_sum =
                reinterpret_cast<blas1::vector4::float32::component_sum_def>(GetProcAddress(library, "scomponentsum4"));
            VERIFY(blas1::vector4::float32::dot);
            blas1::vector4::float32::magnitude =
                reinterpret_cast<blas1::vector4::float32::magnitude_def>(GetProcAddress(library, "smagnitude4"));
            VERIFY(blas1::vector4::float32::magnitude);
            blas1::vector4::float32::component_max =
                reinterpret_cast<blas1::vector4::float32::component_max_def>(GetProcAddress(library, "scomponentmax4"));
            VERIFY(blas1::vector4::float32::component_max);


            blas1::vector4::float64::axpy = reinterpret_cast<blas1::vector4::float64::axpy_def>(GetProcAddress(library, "daxpy4"));
            VERIFY(blas1::vector4::float64::axpy);
            blas1::vector4::float64::scale = reinterpret_cast<blas1::vector4::float64::scale_def>(GetProcAddress(library, "dscale4"));
            VERIFY(blas1::vector4::float64::scale);
            blas1::vector4::float64::copy = reinterpret_cast<blas1::vector4::float64::copy_def>(GetProcAddress(library, "dcopy4"));
            VERIFY(blas1::vector4::float64::copy);
            blas1::vector4::float64::swap = reinterpret_cast<blas1::vector4::float64::swap_def>(GetProcAddress(library, "dswap4"));
            VERIFY(blas1::vector4::float64::swap);
            blas1::vector4::float64::dot = reinterpret_cast<blas1::vector4::float64::dot_def>(GetProcAddress(library, "ddot4"));
            blas1::vector4::float64::component_sum =
                reinterpret_cast<blas1::vector4::float64::component_sum_def>(GetProcAddress(library, "dcomponentsum4"));
            VERIFY(blas1::vector4::float64::dot);
            blas1::vector4::float64::magnitude =
                reinterpret_cast<blas1::vector4::float64::magnitude_def>(GetProcAddress(library, "dmagnitude4"));
            VERIFY(blas1::vector4::float64::magnitude);
            blas1::vector4::float64::component_max =
                reinterpret_cast<blas1::vector4::float64::component_max_def>(GetProcAddress(library, "dcomponentmax4"));
            VERIFY(blas1::vector4::float64::component_max);
            logger.info("Successfully linked vector functions");
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

