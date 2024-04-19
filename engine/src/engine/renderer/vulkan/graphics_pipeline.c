#include "graphics_pipeline.h"
#include <std/core/file.h>
#include <std/core/memory.h>

void create_shader_module(VulkanContext *context, const char *shader_binary_path, VkShaderModule *out) {
    BinaryContents code = {0};
    file_read_binary(shader_binary_path, &code);

    VkShaderModuleCreateInfo shader_module_create_info = {VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
    shader_module_create_info.pCode = code.data;
    shader_module_create_info.codeSize = code.size;

    VK_CHECK(vkCreateShaderModule(context->device.vk_device, &shader_module_create_info, context->allocation_callbacks,
                                  out))

    memory_free(code.data);
}

void create_render_pass(VulkanContext *context, VkRenderPass *out) {
    VkAttachmentDescription color_attachment_description = {};
    color_attachment_description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment_description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment_description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment_description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment_description.format = context->surface.format.format;
    color_attachment_description.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment_description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment_description.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference color_attachment_reference = {};
    color_attachment_reference.attachment = 0;
    color_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_reference;

    VkSubpassDependency dependency = {0};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo render_pass_create_info = {VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
    render_pass_create_info.attachmentCount = 1;
    render_pass_create_info.pAttachments = &color_attachment_description;
    render_pass_create_info.subpassCount = 1;
    render_pass_create_info.pSubpasses = &subpass;
    render_pass_create_info.dependencyCount = 1;
    render_pass_create_info.pDependencies = &dependency;

    VK_CHECK(
            vkCreateRenderPass(context->device.vk_device, &render_pass_create_info, context->allocation_callbacks, out))
}

bool graphics_pipeline_create(VulkanContext *context, GraphicsPipeline *out) {
    create_render_pass(context, &out->render_pass);
    create_shader_module(context, "../shaders/basic.vert.spv", &out->vertex_shader_module);
    create_shader_module(context, "../shaders/basic.frag.spv", &out->fragment_shader_module);

    VkDynamicState dynamic_states[] = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamic_state_create_info = {VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO};
    dynamic_state_create_info.pDynamicStates = dynamic_states;
    dynamic_state_create_info.dynamicStateCount = sizeof(dynamic_states) / sizeof(dynamic_states[0]);

    VkPipelineVertexInputStateCreateInfo vertex_input_create_info = {
            VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
    vertex_input_create_info.vertexBindingDescriptionCount = 0;
    vertex_input_create_info.pVertexBindingDescriptions = 0;
    vertex_input_create_info.vertexAttributeDescriptionCount = 0;
    vertex_input_create_info.pVertexAttributeDescriptions = 0;

    VkPipelineInputAssemblyStateCreateInfo input_assembly_create_info = {
            VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
    input_assembly_create_info.primitiveRestartEnable = false;
    input_assembly_create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    VkPipelineViewportStateCreateInfo viewport_state_create_info = {
            VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
    viewport_state_create_info.scissorCount = 1;
    viewport_state_create_info.viewportCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterization_create_info = {
            VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
    rasterization_create_info.polygonMode = VK_POLYGON_MODE_FILL;
    rasterization_create_info.lineWidth = 1.0f;
    rasterization_create_info.depthBiasEnable = false;
    rasterization_create_info.depthClampEnable = false;
    rasterization_create_info.rasterizerDiscardEnable = false;
    rasterization_create_info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterization_create_info.cullMode = VK_CULL_MODE_BACK_BIT;

    VkPipelineMultisampleStateCreateInfo multisample_create_info = {
            VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
    multisample_create_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisample_create_info.sampleShadingEnable = false;
    multisample_create_info.alphaToOneEnable = false;
    multisample_create_info.alphaToCoverageEnable = false;

    VkPipelineColorBlendAttachmentState color_blend_attachment = {};
    color_blend_attachment.blendEnable = false;
    color_blend_attachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    VkPipelineColorBlendStateCreateInfo color_blend = {VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
    color_blend.logicOpEnable = VK_FALSE;
    color_blend.attachmentCount = 1;
    color_blend.pAttachments = &color_blend_attachment;

    VkPipelineLayoutCreateInfo pipeline_layout_create_info = {VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
    pipeline_layout_create_info.setLayoutCount = 0;
    VK_CHECK(vkCreatePipelineLayout(context->device.vk_device, &pipeline_layout_create_info,
                                    context->allocation_callbacks, &out->pipeline_layout))

    VkPipelineShaderStageCreateInfo vertex_shader_create_info = {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
    vertex_shader_create_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertex_shader_create_info.module = out->vertex_shader_module;
    vertex_shader_create_info.pName = "main";

    VkPipelineShaderStageCreateInfo fragment_shader_create_info = {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
    fragment_shader_create_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragment_shader_create_info.module = out->fragment_shader_module;
    fragment_shader_create_info.pName = "main";

    VkPipelineShaderStageCreateInfo stages[] = {
            vertex_shader_create_info,
            fragment_shader_create_info
    };

    VkGraphicsPipelineCreateInfo create_info = {VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
    create_info.pDynamicState = &dynamic_state_create_info;
    create_info.pInputAssemblyState = &input_assembly_create_info;
    create_info.pRasterizationState = &rasterization_create_info;
    create_info.pMultisampleState = &multisample_create_info;
    create_info.pColorBlendState = &color_blend;
    create_info.pVertexInputState = &vertex_input_create_info;
    create_info.pViewportState = &viewport_state_create_info;
    create_info.renderPass = out->render_pass;
    create_info.stageCount = sizeof(stages) / sizeof(stages[0]);
    create_info.pStages = stages;
    create_info.layout = out->pipeline_layout;

    VK_CHECK(vkCreateGraphicsPipelines(context->device.vk_device, NULL, 1, &create_info, context->allocation_callbacks,
                                       &out->vk_pipeline));
    return true;
}

void graphics_pipeline_destroy(VulkanContext *context, GraphicsPipeline *pipeline) {
    vkDestroyPipelineLayout(context->device.vk_device, pipeline->pipeline_layout, context->allocation_callbacks);
    vkDestroyShaderModule(context->device.vk_device, pipeline->vertex_shader_module, context->allocation_callbacks);
    vkDestroyShaderModule(context->device.vk_device, pipeline->fragment_shader_module, context->allocation_callbacks);
    vkDestroyRenderPass(context->device.vk_device, pipeline->render_pass, context->allocation_callbacks);
    vkDestroyPipeline(context->device.vk_device, pipeline->vk_pipeline, context->allocation_callbacks);
}