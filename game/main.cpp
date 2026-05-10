#include <engine/assets/library.h>
#include <engine/deletion_queue.h>
#include <engine/engine.h>
#include <engine/engine_utils.h>
#include <engine/graphics/pipeline.h>
#include <engine/meta_defines.h>
#include <engine/state/state.h>
#include <memory>
#include <utility>
#include <vector>

class TestPipeline : public engine::StatePipeline {
    public:
        using engine::StatePipeline::StatePipeline;

        virtual auto name() const -> std::string override final {
            return "test pipeline";
        }
        virtual auto build_pipeline(engine::GraphicsEngine& engine,
                                    engine::VulkanDevice& device,
                                    engine::GraphicsRegisteredPipelineDeletionQueue&) -> engine::GraphicsPipelineBuilder override final {
            ZoneScoped;
            auto shader_result = engine::asset::BytecodeShader::load_from_file("assets/shaders/engine/static_triangle.spv").compile(device);
            if (!shader_result.has_value()) {
                engine::crash(ErrorCode::CANNOT_READ_FILE, __LINE__, __func__, __FILE__);
                std::unreachable();
            }
            auto shader            = shader_result.value();

            auto triangle_pipeline = engine::GraphicsPipeline::build()
                                         .layout()
                                         .finish()
                                         .shaders(shader, shader)
                                         .input_topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
                                         .polygon_mode(VK_POLYGON_MODE_FILL)
                                         .cull_mode(VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE)
                                         .set_multisampling_none()
                                         .disable_blending()
                                         .disable_depthtest()
                                         //.color_attachment_format(engine)
                                         .depth_format(VK_FORMAT_UNDEFINED);
            engine.destroy_shader(shader);

            return triangle_pipeline;
        }

    protected:
        virtual auto record_(VkCommandBuffer cmd) -> void override final {
            ZoneScoped;
            vkCmdDraw(cmd, 3, 1, 0, 0);
        }
};

class TestState : public engine::State {
    public:
        virtual auto update() -> void override final {
            engine::g_ENGINE->logger.get(engine::LogNamespaces::GAME).get().info("hiii");
        }

        virtual auto init_pipelines(engine::GraphicsEngine&) -> std::vector<std::unique_ptr<engine::StatePipeline>> override final {
            auto pipelines = std::vector<std::unique_ptr<engine::StatePipeline>>{};
            pipelines.emplace_back(std::make_unique<TestPipeline>(*this));
            return pipelines;
        }
};

int main() {
    engine::Engine engine{};
    engine.state_manager.queue<TestState>();
    engine.run();
    return 0;
}
