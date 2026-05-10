#pragma once
#include "engine/meta_defines.h"

#include <linalg/matrix.h>

namespace ENGINE_NS {
    namespace scene {
        class ICamera {
            public:
                virtual auto perspective() const -> ::linalg::Matrix4<float> = 0;
        };
        class PerspectiveCamera : public ICamera {
            public:
                PerspectiveCamera(float aspect_ratio, float y_fov, float z_near, float z_far);
                PerspectiveCamera(float aspect_ratio, float y_fov, float z_near);

                virtual auto perspective() const -> ::linalg::Matrix4<float> override final;

            private:
                float aspect_ratio_              = 0.f;
                float y_fov_                     = 0.f;
                float z_far_                     = 0.f;
                float z_near_                    = 0.f;
                bool dirty_                      = true;
                ::linalg::Matrix4<float> matrix_ = ::linalg::Matrix4<float>::identity();
        };
        class OrthographicCamera : public ICamera {
            public:
                OrthographicCamera(float left, float right, float top, float bottom, float z_near, float z_far);
                OrthographicCamera(float x_mag, float y_mag, float z_near, float z_far);

                virtual auto perspective() const -> ::linalg::Matrix4<float> override final;

            private:
                float x_mag_                     = 0.f;
                float y_mag_                     = 0.f;
                float z_near_                    = 0.f;
                float z_far_                     = 0.f;
                bool dirty_                      = true;
                ::linalg::Matrix4<float> matrix_ = ::linalg::Matrix4<float>::identity();
        };
    } // namespace scene
} // namespace ENGINE_NS
