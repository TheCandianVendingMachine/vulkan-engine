#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <cstddef>
#include <cstring>

#include "linalg/vector_ops.h"

TEST_CASE("C API component_max uses max symbol", "[blas1][c_api]") {
    const float vf2[] = {-3.0f, 2.0f};
    REQUIRE_THAT(scomponentmax2(vf2), Catch::Matchers::WithinAbs(3.0f, 1e-6f));

    const double vd2[] = {-3.0, 2.0};
    REQUIRE_THAT(dcomponentmax2(vd2), Catch::Matchers::WithinAbs(3.0, 1e-12));

    const float vf3[] = {-3.0f, 2.0f, 1.0f};
    REQUIRE_THAT(scomponentmax3(vf3), Catch::Matchers::WithinAbs(3.0f, 1e-6f));

    const double vd3[] = {-3.0, 2.0, 1.0};
    REQUIRE_THAT(dcomponentmax3(vd3), Catch::Matchers::WithinAbs(3.0, 1e-12));

    const float vf4[] = {-3.0f, 2.0f, 1.0f, 0.5f};
    REQUIRE_THAT(scomponentmax4(vf4), Catch::Matchers::WithinAbs(3.0f, 1e-6f));

    const double vd4[] = {-3.0, 2.0, 1.0, 0.5};
    REQUIRE_THAT(dcomponentmax4(vd4), Catch::Matchers::WithinAbs(3.0, 1e-12));
}

TEST_CASE("C API double swaps use double swap symbols", "[blas1][c_api]") {
    double a3[] = {1.0, 2.0, 3.0};
    double b3[] = {4.0, 5.0, 6.0};
    dswap3(a3, b3);
    REQUIRE_THAT(a3[0], Catch::Matchers::WithinAbs(4.0, 1e-12));
    REQUIRE_THAT(a3[1], Catch::Matchers::WithinAbs(5.0, 1e-12));
    REQUIRE_THAT(a3[2], Catch::Matchers::WithinAbs(6.0, 1e-12));
    REQUIRE_THAT(b3[0], Catch::Matchers::WithinAbs(1.0, 1e-12));
    REQUIRE_THAT(b3[1], Catch::Matchers::WithinAbs(2.0, 1e-12));
    REQUIRE_THAT(b3[2], Catch::Matchers::WithinAbs(3.0, 1e-12));

    double a4[] = {1.0, 2.0, 3.0, 4.0};
    double b4[] = {5.0, 6.0, 7.0, 8.0};
    dswap4(a4, b4);
    REQUIRE_THAT(a4[0], Catch::Matchers::WithinAbs(5.0, 1e-12));
    REQUIRE_THAT(a4[1], Catch::Matchers::WithinAbs(6.0, 1e-12));
    REQUIRE_THAT(a4[2], Catch::Matchers::WithinAbs(7.0, 1e-12));
    REQUIRE_THAT(a4[3], Catch::Matchers::WithinAbs(8.0, 1e-12));
    REQUIRE_THAT(b4[0], Catch::Matchers::WithinAbs(1.0, 1e-12));
    REQUIRE_THAT(b4[1], Catch::Matchers::WithinAbs(2.0, 1e-12));
    REQUIRE_THAT(b4[2], Catch::Matchers::WithinAbs(3.0, 1e-12));
    REQUIRE_THAT(b4[3], Catch::Matchers::WithinAbs(4.0, 1e-12));
}

TEST_CASE("C API Vector4 axpy and scale support aligned and unaligned output", "[blas1][c_api]") {
    const float xf[] = {1.0f, 2.0f, 3.0f, 4.0f};
    const float yf[] = {5.0f, 6.0f, 7.0f, 8.0f};

    alignas(16) float aligned_f[4]{};
    saxpy4(aligned_f, 2.0f, xf, yf);
    REQUIRE_THAT(aligned_f[0], Catch::Matchers::WithinAbs(7.0f, 1e-6f));
    REQUIRE_THAT(aligned_f[3], Catch::Matchers::WithinAbs(16.0f, 1e-6f));

    alignas(16) std::byte unaligned_f_storage[sizeof(float) * 4 + 1]{};
    auto* unaligned_f = reinterpret_cast<float*>(unaligned_f_storage + 1);
    sscale4(unaligned_f, 3.0f, xf);
    float unaligned_f_result[4]{};
    std::memcpy(unaligned_f_result, unaligned_f, sizeof(unaligned_f_result));
    REQUIRE_THAT(unaligned_f_result[0], Catch::Matchers::WithinAbs(3.0f, 1e-6f));
    REQUIRE_THAT(unaligned_f_result[3], Catch::Matchers::WithinAbs(12.0f, 1e-6f));

    const double xd[] = {1.0, 2.0, 3.0, 4.0};
    const double yd[] = {5.0, 6.0, 7.0, 8.0};

    alignas(16) double aligned_d[4]{};
    daxpy4(aligned_d, 2.0, xd, yd);
    REQUIRE_THAT(aligned_d[0], Catch::Matchers::WithinAbs(7.0, 1e-12));
    REQUIRE_THAT(aligned_d[3], Catch::Matchers::WithinAbs(16.0, 1e-12));

    alignas(16) std::byte unaligned_d_storage[sizeof(double) * 4 + 1]{};
    auto* unaligned_d = reinterpret_cast<double*>(unaligned_d_storage + 1);
    dscale4(unaligned_d, 3.0, xd);
    double unaligned_d_result[4]{};
    std::memcpy(unaligned_d_result, unaligned_d, sizeof(unaligned_d_result));
    REQUIRE_THAT(unaligned_d_result[0], Catch::Matchers::WithinAbs(3.0, 1e-12));
    REQUIRE_THAT(unaligned_d_result[3], Catch::Matchers::WithinAbs(12.0, 1e-12));
}
