#pragma once

#include <std/defines.h>
#include "vulkan_types.h"
#include "engine/renderer/data.h"

bool vertex_buffer_create(VulkanContext *context, Vertex *vertices, VertexBuffer *out);

void vertex_buffer_destroy(VulkanContext *context, VertexBuffer *buffer);

void vertex_buffer_copy(VulkanContext *context, VertexBuffer *buffer, Vertex *vertices);