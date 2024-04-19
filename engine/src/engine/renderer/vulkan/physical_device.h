#pragma once

#include "vulkan_types.h"

bool physical_device_select_best(VulkanContext *context);

void physical_device_shutdown(VulkanContext *context);