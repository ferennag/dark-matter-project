#include "graphics_pipeline.h"

void create_render_pass(VulkanContext *context, VkRenderPass *out) {
    VkAttachmentDescription color_attachment_description = {};
    color_attachment_description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment_description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment_description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment_description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment_description.format = context->swap_chain.surface_format.format;
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

    VkRenderPassCreateInfo render_pass_create_info = {VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
    render_pass_create_info.attachmentCount = 1;
    render_pass_create_info.pAttachments = &color_attachment_description;
    render_pass_create_info.subpassCount = 1;
    render_pass_create_info.pSubpasses = &subpass;

    VK_CHECK(
            vkCreateRenderPass(context->device.vk_device, &render_pass_create_info, context->allocation_callbacks, out))
}

bool graphics_pipeline_create(VulkanContext *context, GraphicsPipeline *out) {
    create_render_pass(context, &out->render_pass);

    VkPipelineColorBlendStateCreateInfo color_blend = {VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
    color_blend.attachmentCount = 0;
    color_blend.logicOpEnable = VK_FALSE;

    VkGraphicsPipelineCreateInfo create_info = {VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
    create_info.pColorBlendState = &color_blend;

    return true;
}

void graphics_pipeline_destroy(VulkanContext *context, GraphicsPipeline *out) {
    vkDestroyRenderPass(context->device.vk_device, out->render_pass, context->allocation_callbacks);
}