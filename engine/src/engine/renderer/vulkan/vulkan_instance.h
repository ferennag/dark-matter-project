#pragma once

#include "vulkan_types.h"
#include "engine/platform/platform.h"

bool
vulkan_instance_create(VulkanContext *context, PlatformState *platform_state, const char *app_name);

void vulkan_instance_destroy(VulkanContext *context);