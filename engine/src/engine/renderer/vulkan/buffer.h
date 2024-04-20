#pragma once

#include <std/defines.h>
#include "vulkan_types.h"
#include "engine/renderer/data.h"

bool buffer_create(VulkanContext *context, VkBufferUsageFlags usage, u32 size, Buffer *out);

void buffer_destroy(VulkanContext *context, Buffer *buffer);

void buffer_copy(VulkanContext *context, Buffer *buffer, Vertex *vertices);