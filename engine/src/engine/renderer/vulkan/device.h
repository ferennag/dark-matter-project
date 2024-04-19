#pragma once

#include "vulkan_types.h"

bool device_create(VulkanContext *context);

void device_destroy(VulkanContext *context);

bool is_device_extension_available(Device *device, const char *name);

QueueFamily *find_queue_family(VulkanContext *context, QueueCapability capability);