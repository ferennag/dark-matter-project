#pragma once

#include <std/defines.h>
#include "vulkan_types.h"

GraphicsPipelineConfiguration *pipeline_config_create(const char *vertexPath, const char *fragmentPath);

void pipeline_config_destroy(GraphicsPipelineConfiguration *config);

void pipeline_config_add_va_description(GraphicsPipelineConfiguration *config, VkFormat format, u32 binding,
                                        u32 location, u32 offset);

void pipeline_config_add_va_binding(GraphicsPipelineConfiguration *config, VkVertexInputRate input_rate,
                                    u32 binding, u32 stride);