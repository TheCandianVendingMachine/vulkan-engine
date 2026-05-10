#pragma once
#include "engine/graphics/scene/renderable.h"
#include "engine/graphics/scene/types.h"
#include "engine/meta_defines.h"

#include <linalg/matrix.h>
#include <linalg/vector.h>
#include <memory>
#include <vector>


namespace ENGINE_NS {
    namespace scene {
        struct Node {
                ::linalg::Vector3<float> dimensions{};
                ::linalg::Matrix4<float> global_transform = ::linalg::Matrix4<float>::identity();
                ::linalg::Matrix4<float> local_transform  = ::linalg::Matrix4<float>::identity();
                std::unique_ptr<IRenderable> renderable   = nullptr;
                NodeUid id{};
                std::vector<NodeUid> children{};

                auto in_frustum(const ::linalg::Matrix4<float>& vp) const -> bool;
        };
    } // namespace scene
} // namespace ENGINE_NS
