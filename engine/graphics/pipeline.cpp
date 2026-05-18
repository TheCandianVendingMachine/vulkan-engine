#include "engine/graphics/pipeline.h"

#include "engine/graphics/util.h"
#include "engine/graphics/vulkan.h"
#include "engine/meta_defines.h"

#include <Volk/volk.h>
#include <vulkan/vulkan_core.h>

#include <type_traits>


auto ENGINE_NS::GraphicsPipeline::build() -> GraphicsPipelineBuilder {
    return GraphicsPipelineBuilder();
}

ENGINE_NS::GraphicsPipeline::GraphicsPipeline(const GraphicsPipeline& rhs) : pipeline_(rhs.pipeline_), layout_(rhs.layout_) {
}

ENGINE_NS::GraphicsPipeline::GraphicsPipeline(GraphicsPipeline&& rhs) noexcept :
    pipeline_(std::move(rhs.pipeline_)), layout_(std::move(rhs.layout_)) {
}

auto ENGINE_NS::GraphicsPipeline::operator=(const GraphicsPipeline& rhs) -> GraphicsPipeline& {
    if (this != &rhs) {
        pipeline_ = rhs.pipeline_;
        layout_   = rhs.layout_;
    }
    return *this;
}

auto ENGINE_NS::GraphicsPipeline::operator=(GraphicsPipeline&& rhs) noexcept -> GraphicsPipeline& {
    if (this != &rhs) {
        pipeline_ = std::move(rhs.pipeline_);
        layout_   = std::move(rhs.layout_);
    }
    return *this;
}

ENGINE_NS::GraphicsPipeline::GraphicsPipeline(VulkanDevice& device,
                                              VkGraphicsPipelineCreateInfo pipeline_info,
                                              VkPipelineLayoutCreateInfo layout_info) {
    VK_CHECK(vkCreatePipelineLayout(device.device, &layout_info, nullptr, &layout_));
    pipeline_info.layout = layout_;

    VK_CHECK(vkCreateGraphicsPipelines(device.device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &pipeline_));
}

ENGINE_NS::GraphicsPipelineBuilder::GraphicsPipelineBuilder() {
    input_assembly_.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    rasterizer_.sType     = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    multisampling_.sType  = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    depth_stencil_.sType  = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    render_info_.sType    = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
}

auto ENGINE_NS::GraphicsPipelineBuilder::shaders(asset::CompiledShader vertex_shader, asset::CompiledShader pixel_shader)
    -> GraphicsPipelineBuilder& {
    shader_stages_.clear();
    shader_stages_.push_back(pipeline_shader_stage_create_info(VK_SHADER_STAGE_VERTEX_BIT, vertex_shader.shader, "vertexMain"));
    shader_stages_.push_back(pipeline_shader_stage_create_info(VK_SHADER_STAGE_FRAGMENT_BIT, pixel_shader.shader, "pixelMain"));
    return *this;
}

auto ENGINE_NS::GraphicsPipelineBuilder::input_topology(VkPrimitiveTopology topology) -> GraphicsPipelineBuilder& {
    input_assembly_.topology               = topology;
    input_assembly_.primitiveRestartEnable = VK_FALSE;
    return *this;
}

auto ENGINE_NS::GraphicsPipelineBuilder::polygon_mode(VkPolygonMode mode) -> GraphicsPipelineBuilder& {
    rasterizer_.polygonMode = mode;
    rasterizer_.lineWidth   = 1.f;
    return *this;
}

auto ENGINE_NS::GraphicsPipelineBuilder::cull_mode(VkCullModeFlags cull_mode, VkFrontFace front_face) -> GraphicsPipelineBuilder& {
    rasterizer_.cullMode  = cull_mode;
    rasterizer_.frontFace = front_face;
    return *this;
}

auto ENGINE_NS::GraphicsPipelineBuilder::color_attachment_format(VkFormat format) -> GraphicsPipelineBuilder& {
    colour_attachment_format_            = format;
    render_info_.colorAttachmentCount    = 1;
    render_info_.pColorAttachmentFormats = &colour_attachment_format_;
    return *this;
}

auto ENGINE_NS::GraphicsPipelineBuilder::depth_format(VkFormat format) -> GraphicsPipelineBuilder& {
    render_info_.depthAttachmentFormat = format;
    return *this;
}

auto ENGINE_NS::GraphicsPipelineBuilder::disable_depthtest() -> GraphicsPipelineBuilder& {
    depth_stencil_.depthTestEnable       = VK_FALSE;
    depth_stencil_.depthWriteEnable      = VK_FALSE;
    depth_stencil_.depthCompareOp        = VK_COMPARE_OP_NEVER;
    depth_stencil_.depthBoundsTestEnable = VK_FALSE;
    depth_stencil_.stencilTestEnable     = VK_FALSE;
    depth_stencil_.front                 = {};
    depth_stencil_.back                  = {};
    depth_stencil_.minDepthBounds        = 0.f;
    depth_stencil_.maxDepthBounds        = 1.f;
    return *this;
}

auto ENGINE_NS::GraphicsPipelineBuilder::set_multisampling_none() -> GraphicsPipelineBuilder& {
    multisampling_.sampleShadingEnable   = VK_FALSE;
    multisampling_.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
    multisampling_.minSampleShading      = 1.0f;
    multisampling_.pSampleMask           = nullptr;
    multisampling_.alphaToCoverageEnable = VK_FALSE;
    multisampling_.alphaToOneEnable      = VK_FALSE;
    return *this;
}

auto ENGINE_NS::GraphicsPipelineBuilder::disable_blending() -> GraphicsPipelineBuilder& {
    color_blend_attachment_.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    color_blend_attachment_.blendEnable = VK_FALSE;
    return *this;
}

auto ENGINE_NS::GraphicsPipelineBuilder::layout() -> PipelineLayoutBuilder<GraphicsPipelineBuilder> {
    return PipelineLayoutBuilder<GraphicsPipelineBuilder>(*this);
}

auto ENGINE_NS::GraphicsPipelineBuilder::finish(VulkanDevice& device) -> GraphicsPipeline {
    VkPipelineViewportStateCreateInfo viewport_state = {};
    viewport_state.sType                             = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state.viewportCount                     = 1;
    viewport_state.scissorCount                      = 1;

    VkPipelineColorBlendStateCreateInfo colour_blending = {};
    colour_blending.sType                               = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colour_blending.logicOpEnable                       = VK_FALSE;
    colour_blending.logicOp                             = VK_LOGIC_OP_COPY;
    colour_blending.attachmentCount                     = 1;
    colour_blending.pAttachments                        = &color_blend_attachment_;

    VkPipelineVertexInputStateCreateInfo vertex_input_info = {};
    vertex_input_info.sType                                = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    VkDynamicState state[]                        = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
    VkPipelineDynamicStateCreateInfo dynamic_info = {};
    dynamic_info.sType                            = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_info.pDynamicStates                   = state;
    dynamic_info.dynamicStateCount                = sizeof(state) / sizeof(state[0]);

    VkGraphicsPipelineCreateInfo pipeline_info = {};
    pipeline_info.sType                        = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_info.pNext                        = &render_info_;
    pipeline_info.stageCount                   = static_cast<std::uint32_t>(shader_stages_.size());
    pipeline_info.pStages                      = shader_stages_.data();
    pipeline_info.pVertexInputState            = &vertex_input_info;
    pipeline_info.pInputAssemblyState          = &input_assembly_;
    pipeline_info.pViewportState               = &viewport_state;
    pipeline_info.pRasterizationState          = &rasterizer_;
    pipeline_info.pMultisampleState            = &multisampling_;
    pipeline_info.pColorBlendState             = &colour_blending;
    pipeline_info.pDepthStencilState           = &depth_stencil_;
    pipeline_info.pDynamicState                = &dynamic_info;

    return GraphicsPipeline(device, pipeline_info, pipeline_layout_);
}

ENGINE_NS::ComputePipelineBuilder::ComputePipelineBuilder() {
    shader_stage_.sType    = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    pipeline_layout_.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
}
auto ENGINE_NS::ComputePipeline::build() -> ComputePipelineBuilder {
    return ComputePipelineBuilder();
}

ENGINE_NS::ComputePipeline::ComputePipeline(const ComputePipeline& rhs) : pipeline_(rhs.pipeline_), layout_(rhs.layout_) {
}

ENGINE_NS::ComputePipeline::ComputePipeline(ComputePipeline&& rhs) noexcept :
    pipeline_(std::move(rhs.pipeline_)), layout_(std::move(rhs.layout_)) {
}

auto ENGINE_NS::ComputePipeline::operator=(const ComputePipeline& rhs) -> ComputePipeline& {
    if (this != &rhs) {
        pipeline_ = rhs.pipeline_;
        layout_   = rhs.layout_;
    }
    return *this;
}

auto ENGINE_NS::ComputePipeline::operator=(ComputePipeline&& rhs) noexcept -> ComputePipeline& {
    if (this != &rhs) {
        pipeline_ = std::move(rhs.pipeline_);
        layout_   = std::move(rhs.layout_);
    }
    return *this;
}

ENGINE_NS::ComputePipeline::ComputePipeline(VulkanDevice& device,
                                            VkComputePipelineCreateInfo pipeline_info,
                                            VkPipelineLayoutCreateInfo layout_info) {
    VK_CHECK(vkCreatePipelineLayout(device.device, &layout_info, nullptr, &layout_));
    pipeline_info.layout = layout_;

    VK_CHECK(vkCreateComputePipelines(device.device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &pipeline_));
}

auto ENGINE_NS::ComputePipelineBuilder::shader(asset::CompiledShader compute_shader) -> ComputePipelineBuilder& {
    shader_stage_ = pipeline_shader_stage_create_info(VK_SHADER_STAGE_COMPUTE_BIT, compute_shader.shader, "main");
    return *this;
}

auto ENGINE_NS::ComputePipelineBuilder::layout() -> PipelineLayoutBuilder<ComputePipelineBuilder> {
    return PipelineLayoutBuilder<ComputePipelineBuilder>(*this);
}

auto ENGINE_NS::ComputePipelineBuilder::finish(VulkanDevice& device) -> ComputePipeline {
    VkComputePipelineCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    create_info.stage = shader_stage_;
    return ComputePipeline(device, create_info, pipeline_layout_);
}
