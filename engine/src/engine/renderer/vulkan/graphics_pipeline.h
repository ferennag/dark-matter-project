#pragma once

#include "vulkan_types.h"

bool graphics_pipeline_create(VulkanContext *context, GraphicsPipelineConfiguration *configuration, GraphicsPipeline *out);

void graphics_pipeline_destroy(VulkanContext *context, GraphicsPipeline *out);