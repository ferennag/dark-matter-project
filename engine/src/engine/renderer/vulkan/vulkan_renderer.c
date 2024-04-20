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

    vkDeviceWaitIdle(context->device.vk_device);

    vkDestroyFence(context->device.vk_device, context->in_flight_fence, context->allocation_callbacks);
    vkDestroySemaphore(context->device.vk_device, context->render_finished_semaphore, context->allocation_callbacks);
    vkDestroySemaphore(context->device.vk_device, context->image_available_semaphore, context->allocation_callbacks);

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

void record_commands(VulkanContext *context, RenderPacket *packet, const u32 image_index) {
    command_buffer_begin(&context->graphics_queue.command_buffer);

    VkClearValue clear_color = {{{0.02f, 0.02f, 0.02f, 1.0f}}};

    VkRenderPassBeginInfo render_pass_info = {VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
    render_pass_info.renderPass = context->graphics_pipeline.render_pass;
    render_pass_info.framebuffer = context->swap_chain.framebuffers[image_index];
    render_pass_info.renderArea.offset = (VkOffset2D) {0, 0};
    render_pass_info.renderArea.extent = context->surface.capabilities.currentExtent;
    render_pass_info.pClearValues = &clear_color;
    render_pass_info.clearValueCount = 1;

    vkCmdBeginRenderPass(context->graphics_queue.command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(context->graphics_queue.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      context->graphics_pipeline.vk_pipeline);

    VkViewport viewport = {0};
    viewport.width = context->surface.capabilities.currentExtent.width;
    viewport.height = context->surface.capabilities.currentExtent.height;
    viewport.minDepth = 0;
    viewport.maxDepth = 1;
    vkCmdSetViewport(context->graphics_queue.command_buffer, 0, 1, &viewport);

    VkRect2D scissor = {0};
    scissor.offset = (VkOffset2D) {0, 0};
    scissor.extent = context->surface.capabilities.currentExtent;
    vkCmdSetScissor(context->graphics_queue.command_buffer, 0, 1, &scissor);

    vkCmdDraw(context->graphics_queue.command_buffer, 3, 1, 0, 0);

    vkCmdEndRenderPass(context->graphics_queue.command_buffer);
    command_buffer_end(&context->graphics_queue.command_buffer);
}

void vulkan_render(RendererBackend *backend, RenderPacket *packet) {
    VulkanContext *context = backend->renderer_context;

    VK_CHECK(vkWaitForFences(context->device.vk_device, 1, &context->in_flight_fence, true, UINT64_MAX))
    VK_CHECK(vkResetFences(context->device.vk_device, 1, &context->in_flight_fence))

    u32 image_index = 0;
    VK_CHECK(vkAcquireNextImageKHR(context->device.vk_device, context->swap_chain.vk_swapchain, UINT64_MAX,
                                   context->image_available_semaphore, NULL, &image_index))

    vkResetCommandBuffer(context->graphics_queue.command_buffer, 0);
    record_commands(context, packet, image_index);

    VkSemaphore waitSemaphores[] = {context->image_available_semaphore};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    VkSubmitInfo submitInfo = {VK_STRUCTURE_TYPE_SUBMIT_INFO};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &context->graphics_queue.command_buffer;

    VkSemaphore signalSemaphores[] = {context->render_finished_semaphore};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    VK_CHECK(vkQueueSubmit(context->graphics_queue.vk_queue, 1, &submitInfo, context->in_flight_fence))

    VkPresentInfoKHR present_info = {VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = signalSemaphores;
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &context->swap_chain.vk_swapchain;
    present_info.pImageIndices = &image_index;

    VK_CHECK(vkQueuePresentKHR(context->present_queue.vk_queue, &present_info))
}

void create_fence(VulkanContext *context, VkFence *out) {
    VkFenceCreateInfo create_info = {VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
    create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    VK_CHECK(vkCreateFence(context->device.vk_device, &create_info, context->allocation_callbacks, out))
}

void create_semaphore(VulkanContext *context, VkSemaphore *out) {
    VkSemaphoreCreateInfo create_info = {VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
    VK_CHECK(vkCreateSemaphore(context->device.vk_device, &create_info, context->allocation_callbacks, out))
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
    vkGetDeviceQueue(context->device.vk_device, graphics_family->index, 0, &context->graphics_queue.vk_queue);
    command_buffer_create(context, graphics_family, &context->graphics_queue.command_buffer);

    QueueFamily *present_family = find_queue_family(context, QUEUE_PRESENT);
    vkGetDeviceQueue(context->device.vk_device, present_family->index, 0, &context->present_queue.vk_queue);
    command_buffer_create(context, present_family, &context->present_queue.command_buffer);

    create_fence(context, &context->in_flight_fence);
    create_semaphore(context, &context->image_available_semaphore);
    create_semaphore(context, &context->render_finished_semaphore);

    return true;
}