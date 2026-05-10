#pragma once
#include "engine/graphics/scene/camera.h"
#include "engine/graphics/scene/node.h"
#include "engine/graphics/scene/types.h"
#include "engine/meta_defines.h"
#include "engine/pool.h"
#include "engine/pool/types.h"

#include <robin-map/robin_map.h>
#include <vector>

namespace ENGINE_NS {
    namespace scene {
        class Scene {
            public:
                auto get_visible_nodes(const ICamera& camera, const ::linalg::Matrix4<float> transform) -> std::vector<Node*>;

            private:
                NodeUid next_node_id_{};

                std::vector<Handle> roots_{};
                Pool<Node> nodes_{};
                tsl::robin_map<NodeUid, Handle> node_handle_map_{};
        };
    } // namespace scene
} // namespace ENGINE_NS
