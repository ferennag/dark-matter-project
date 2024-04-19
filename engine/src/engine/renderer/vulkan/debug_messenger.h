#pragma once

#include "vulkan_types.h"

bool debug_messenger_create(VulkanContext *context);

void debug_messenger_destroy(VulkanContext *context);