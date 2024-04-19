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

    return true;
}