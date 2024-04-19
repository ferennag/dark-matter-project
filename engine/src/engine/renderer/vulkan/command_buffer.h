#pragma once

#include "vulkan_types.h"

void command_buffer_create(VulkanContext *context, QueueFamily *family, VkCommandBuffer *out);

void command_buffer_begin(VkCommandBuffer *buffer);

void command_buffer_end(VkCommandBuffer *buffer);