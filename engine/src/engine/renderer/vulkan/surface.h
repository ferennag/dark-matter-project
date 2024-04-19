#pragma once

#include "vulkan_types.h"
#include "engine/platform/platform.h"

bool surface_create(PlatformState *platform_state, VulkanContext *context);

void surface_destroy(VulkanContext *context);
