#pragma once
#include "engine/assets/library.h"
#include "engine/graphics/vulkan.h"

#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <vector>


namespace ENGINE_NS {
    template <class TFrom>
    class PipelineLayoutBuilder {
        public:
            using Contained = TFrom;
            auto finish() -> Contained& {
                VkPipelineLayoutCreateInfo layout_info = pipeline_layout_create_info();
                layout_info.pPushConstantRanges        = push_constants_.data();
                layout_info.pushConstantRangeCount     = static_cast<std::uint32_t>(push_constants_.size());
                layout_info.pSetLayouts                = descriptor_set_layouts_.data();
                layout_info.setLayoutCount             = static_cast<std::uint32_t>(descriptor_set_layouts_.size());
                from_.pipeline_layout_                 = layout_info;
                return from_;
            }

            auto push_constant_range(VkPushConstantRange range) -> PipelineLayoutBuilder<Contained>& {
                push_constants_.push_back(range);
                return *this;
            }

            auto add_set_layout(VulkanDescriptorSetLayout& layout) -> PipelineLayoutBuilder<Contained>& {
                descriptor_set_layouts_.push_back(layout.layout);
                return *this;
            }

            PipelineLayoutBuilder(PipelineLayoutBuilder&& other) noexcept :
                push_constants_(std::move(other.push_constants_)), descriptor_set_layouts_(std::move(other.descriptor_set_layouts_)),
                from_(other.from_) {
            }


        private:
            std::vector<VkPushConstantRange> push_constants_;
            std::vector<VkDescriptorSetLayout> descriptor_set_layouts_;
            Contained& from_;

            friend Contained;
            PipelineLayoutBuilder(Contained& from) : from_(from) {
            }
    };

    class VulkanDevice;

    class ComputePipeline;
    class ComputePipelineBuilder {
        public:
            auto shader(asset::CompiledShader compute_shader) -> ComputePipelineBuilder&;
            auto layout() -> PipelineLayoutBuilder<ComputePipelineBuilder>&;
            auto finish(VulkanDevice& device) -> ComputePipeline;

            ComputePipelineBuilder(ComputePipelineBuilder&& rhs) noexcept = default;

        private:
            VkPipelineShaderStageCreateInfo shader_stage_ = {};
            VkPipelineLayoutCreateInfo pipeline_layout_   = {};
            PipelineLayoutBuilder<ComputePipelineBuilder> layout_builder_;

            ComputePipelineBuilder();
            friend class ComputePipeline;
            friend class PipelineLayoutBuilder<ComputePipelineBuilder>;
    };

    class ComputePipeline {
        public:
            ComputePipeline() = default;
            static auto build() -> ComputePipelineBuilder;

            ComputePipeline(const ComputePipeline& rhs);
            ComputePipeline(ComputePipeline&& rhs) noexcept;


            auto operator=(const ComputePipeline& rhs) -> ComputePipeline&;
            auto operator=(ComputePipeline&& rhs) noexcept -> ComputePipeline&;

            const VkPipeline& pipeline     = pipeline_;
            const VkPipelineLayout& layout = layout_;

        private:
            VkPipeline pipeline_     = VK_NULL_HANDLE;
            VkPipelineLayout layout_ = VK_NULL_HANDLE;

            ComputePipeline(VulkanDevice& device, VkComputePipelineCreateInfo pipeline_info, VkPipelineLayoutCreateInfo layout_info);
            friend class ComputePipelineBuilder;
    };

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
            auto layout() -> PipelineLayoutBuilder<GraphicsPipelineBuilder>&;

            auto finish(VulkanDevice& device) -> GraphicsPipeline;

            GraphicsPipelineBuilder(GraphicsPipelineBuilder&& rhs) noexcept = default;

        private:
            std::vector<VkPipelineShaderStageCreateInfo> shader_stages_;

            VkPipelineInputAssemblyStateCreateInfo input_assembly_{};
            VkPipelineRasterizationStateCreateInfo rasterizer_{};
            VkPipelineColorBlendAttachmentState color_blend_attachment_{};
            VkPipelineMultisampleStateCreateInfo multisampling_{};
            VkPipelineLayoutCreateInfo pipeline_layout_{};
            VkPipelineDepthStencilStateCreateInfo depth_stencil_{};
            VkPipelineRenderingCreateInfo render_info_{};
            VkFormat colour_attachment_format_{};
            PipelineLayoutBuilder<GraphicsPipelineBuilder> layout_builder_;

            GraphicsPipelineBuilder();
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
