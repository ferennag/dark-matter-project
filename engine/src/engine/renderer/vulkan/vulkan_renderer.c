#include "vulkan_renderer.h"
#include <std/core/memory.h>
#include <std/core/logger.h>

#include "vulkan_types.h"
#include "vulkan_instance.h"
#include "debug_messenger.h"
#include "physical_device.h"
#include "device.h"
#include "swapchain.h"
#include "graphics_pipeline.h"
#include "surface.h"
#include "command_buffer.h"

bool vulkan_init_context(VulkanContext *context, PlatformState *platform_state, const char *app_name);

bool vulkan_initialize(RendererBackend *backend, PlatformState *platform_state, const char *app_name) {
    VulkanContext *context = memory_alloc(sizeof(VulkanContext));
    backend->renderer_context = context;

    if (!vulkan_init_context(context, platform_state, app_name)) {
        LOG_ERROR("Failed to initialize the vulkan context.");
        return false;
    }

    LOG_INFO("Successfully initialized Vulkan.");
    return true;
}

void vulkan_shutdown(RendererBackend *backend) {
    VulkanContext *context = backend->renderer_context;

    graphics_pipeline_destroy(context, &context->graphics_pipeline);
    swapchain_destroy(context, &context->swap_chain);
    device_destroy(context);
    surface_destroy(context);
    physical_device_shutdown(context);
    debug_messenger_destroy(context);
    vulkan_instance_destroy(context);

    memory_free((VulkanContext *) backend->renderer_context);
    backend->renderer_context = NULL;
    LOG_INFO("Vulkan shutdown complete.");
}

void vulkan_surface_resized(RendererBackend *backend, u32 width, u32 height) {
}

void vulkan_render(RendererBackend *backend, RenderPacket *packet) {
    const int image_index = 0;

    VulkanContext *context = backend->renderer_context;

    command_buffer_begin(&context->graphics_buffer);

    VkClearValue clear_color = {.color = {{0.05f, 0.05f, 0.05f, 1.0f}}};

    VkRenderPassBeginInfo render_pass_info = {VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
    render_pass_info.renderPass = context->graphics_pipeline.render_pass;
    render_pass_info.framebuffer = context->swap_chain.framebuffers[image_index];
    render_pass_info.renderArea.offset = (VkOffset2D) {0, 0};
    render_pass_info.renderArea.extent = context->surface.capabilities.currentExtent;
    render_pass_info.pClearValues = &clear_color;
    render_pass_info.clearValueCount = 1;

    vkCmdBeginRenderPass(context->graphics_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(context->graphics_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      context->graphics_pipeline.vk_pipeline);

    VkViewport viewport = {0};
    viewport.width = context->surface.capabilities.currentExtent.width;
    viewport.height = context->surface.capabilities.currentExtent.height;
    viewport.minDepth = 0;
    viewport.maxDepth = 1;
    vkCmdSetViewport(context->graphics_buffer, 0, 1, &viewport);

    VkRect2D scissor = {0};
    scissor.offset = (VkOffset2D) {0, 0};
    scissor.extent = context->surface.capabilities.currentExtent;
    vkCmdSetScissor(context->graphics_buffer, 0, 1, &scissor);

    vkCmdDraw(context->graphics_buffer, 3, 0, 0, 0);

    vkCmdEndRenderPass(context->graphics_buffer);
    command_buffer_end(&context->graphics_buffer);
}

bool vulkan_init_context(VulkanContext *context, PlatformState *platform_state, const char *app_name) {
    if (!vulkan_instance_create(context, platform_state, app_name)) {
        LOG_ERROR("Failed to create Vulkan instance");
        return false;
    }

    if (!debug_messenger_create(context)) {
        LOG_ERROR("Failed to create Vulkan debug messenger");
        return false;
    }

    if (!physical_device_select_best(context)) {
        LOG_ERROR("Failed to select a physical device");
        return false;
    }

    if (!surface_create(platform_state, context)) {
        return false;
    }

    if (!device_create(context)) {
        LOG_ERROR("Failed to create logical device");
        return false;
    }

    if (!graphics_pipeline_create(context, &context->graphics_pipeline)) {
        LOG_ERROR("Failed to create graphics pipeline");
        return false;
    }

    if (!swapchain_create(context, &context->swap_chain)) {
        LOG_ERROR("Failed to create swap chain");
        return false;
    }

    QueueFamily *graphics_family = find_queue_family(context, QUEUE_GRAPHICS);
    command_buffer_create(context, graphics_family, &context->graphics_buffer);
    QueueFamily *present_family = find_queue_family(context, QUEUE_PRESENT);
    command_buffer_create(context, present_family, &context->present_buffer);

    return true;
}