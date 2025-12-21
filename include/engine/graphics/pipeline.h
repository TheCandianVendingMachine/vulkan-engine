#pragma once
#include "engine/assets/library.h"
#include "engine/engine_utils.h"
#define VK_NO_PROTOTYPES
#include <vector>
#include <vulkan/vulkan_core.h>

namespace ENGINE_NS {
    template <typename TFrom>
    class PipelineLayoutBuilder {};

    struct ComputePipeline {
            VkPipeline pipeline              = VK_NULL_HANDLE;
            VkPipelineLayout pipeline_layout = VK_NULL_HANDLE;
    };

    class GraphicsPipelineBuilder;
    template <>
    class PipelineLayoutBuilder<GraphicsPipelineBuilder> {
        public:
            auto finish() -> GraphicsPipelineBuilder&;

            PipelineLayoutBuilder<GraphicsPipelineBuilder>(const PipelineLayoutBuilder<GraphicsPipelineBuilder>& rhs);
            PipelineLayoutBuilder<GraphicsPipelineBuilder>(PipelineLayoutBuilder<GraphicsPipelineBuilder>&& rhs) noexcept;

        private:
            GraphicsPipelineBuilder& from_;

            friend GraphicsPipelineBuilder;
            PipelineLayoutBuilder(GraphicsPipelineBuilder& from);
    };

    class VulkanDevice;
    class GraphicsPipeline;
    class GraphicsPipelineBuilder {
        public:
            auto shaders(asset::CompiledShader vertex_shader, asset::CompiledShader pixel_shader) -> GraphicsPipelineBuilder&;
            auto input_topology(VkPrimitiveTopology topology) -> GraphicsPipelineBuilder&;
            auto polygon_mode(VkPolygonMode mode) -> GraphicsPipelineBuilder&;
            auto cull_mode(VkCullModeFlags cull_mode, VkFrontFace front_face) -> GraphicsPipelineBuilder&;
            auto color_attachment_format(VkFormat format) -> GraphicsPipelineBuilder&;
            auto depth_format(VkFormat format) -> GraphicsPipelineBuilder&;
            auto disable_depthtest() -> GraphicsPipelineBuilder&;
            auto set_multisampling_none() -> GraphicsPipelineBuilder&;
            auto disable_blending() -> GraphicsPipelineBuilder&;
            auto layout() -> PipelineLayoutBuilder<GraphicsPipelineBuilder>;

            auto finish(VulkanDevice& device) -> GraphicsPipeline;

            GraphicsPipelineBuilder(const GraphicsPipelineBuilder& rhs);
            GraphicsPipelineBuilder(GraphicsPipelineBuilder&& rhs) noexcept;

        private:
            std::vector<VkPipelineShaderStageCreateInfo> shader_stages_{};

            VkPipelineInputAssemblyStateCreateInfo input_assembly_{.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
            VkPipelineRasterizationStateCreateInfo rasterizer_{.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
            VkPipelineColorBlendAttachmentState color_blend_attachment_{};
            VkPipelineMultisampleStateCreateInfo multisampling_{.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
            VkPipelineLayoutCreateInfo pipeline_layout_{};
            VkPipelineDepthStencilStateCreateInfo depth_stencil_{.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO};
            VkPipelineRenderingCreateInfo render_info_{.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO};
            VkFormat colour_attachment_format_{};

            GraphicsPipelineBuilder() = default;
            friend class GraphicsPipeline;
            friend class PipelineLayoutBuilder<GraphicsPipelineBuilder>;
    };
    class GraphicsPipeline {
        public:
            GraphicsPipeline() = default;
            static auto build() -> GraphicsPipelineBuilder;

            GraphicsPipeline(const GraphicsPipeline& rhs);
            GraphicsPipeline(GraphicsPipeline&& rhs) noexcept;


            auto operator=(const GraphicsPipeline& rhs) -> GraphicsPipeline&;
            auto operator=(GraphicsPipeline&& rhs) noexcept -> GraphicsPipeline&;

            const VkPipeline& pipeline     = pipeline_;
            const VkPipelineLayout& layout = layout_;

        private:
            VkPipeline pipeline_     = VK_NULL_HANDLE;
            VkPipelineLayout layout_ = VK_NULL_HANDLE;

            GraphicsPipeline(VulkanDevice& device, VkGraphicsPipelineCreateInfo pipeline_info, VkPipelineLayoutCreateInfo layout_info);
            friend class GraphicsPipelineBuilder;
    };
} // namespace ENGINE_NS
