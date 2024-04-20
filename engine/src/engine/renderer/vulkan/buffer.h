#pragma once

#include <std/defines.h>
#include "vulkan_types.h"
#include "engine/renderer/data.h"

bool
buffer_create(VulkanContext *context, VkBufferUsageFlags usage, VkMemoryPropertyFlags memory_property_flags, u32 size,
              Buffer *out);

void buffer_destroy(VulkanContext *context, Buffer *buffer);

void buffer_copy(VulkanContext *context, Buffer *buffer, void *in_data);

bool vertex_buffer_create(VulkanContext *context, Vertex *vertices, VertexBuffer *out);

void vertex_buffer_destroy(VulkanContext *context, VertexBuffer *buffer);

void vertex_buffer_copy(VulkanContext *context, VertexBuffer *buffer, Vertex *vertices);

bool index_buffer_create(VulkanContext *context, u16 *indices, IndexBuffer *out);

void index_buffer_destroy(VulkanContext *context, IndexBuffer *buffer);

void index_buffer_copy(VulkanContext *context, IndexBuffer *buffer, u16 *indices);