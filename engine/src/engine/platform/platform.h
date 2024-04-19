#pragma once

#include "std/defines.h"

#ifndef RENDERER
#define RENDERER VULKAN
#endif

typedef struct VulkanContext VulkanContext;
typedef struct RendererBackend RendererBackend;

typedef struct PlatformState {
    void *internal_state;
} PlatformState;

bool platform_startup(PlatformState *platform_state, const char *app_name);

void platform_shutdown(PlatformState *platform_state);

bool platform_handle_events(PlatformState *platform_state);

void platform_sleep(u32 ms);

#if RENDERER == VULKAN

#include <vulkan/vulkan.h>

bool platform_required_vulkan_extensions(PlatformState *platform_state, const char ***out_extension_names);

bool platform_vulkan_create_surface(PlatformState *platform_state, VulkanContext *context, VkSurfaceKHR *out_surface);

#endif