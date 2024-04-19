#pragma once

#include "vulkan_types.h"

bool graphics_pipeline_create(VulkanContext *context, GraphicsPipeline *out);

void graphics_pipeline_destroy(VulkanContext *context, GraphicsPipeline *out);