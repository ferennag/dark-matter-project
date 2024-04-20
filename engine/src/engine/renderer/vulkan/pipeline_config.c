#include "pipeline_config.h"
#include <std/core/memory.h>
#include <std/containers/darray.h>

GraphicsPipelineConfiguration *pipeline_config_create(const char *vertexPath, const char *fragmentPath) {
    GraphicsPipelineConfiguration *config = memory_alloc(sizeof(GraphicsPipelineConfiguration));

    config->vertex_shader = vertexPath;
    config->fragment_shader = fragmentPath;
    config->vertex_attribute_descriptions = darray_create(VkVertexInputAttributeDescription);
    config->vertex_binding_descriptions = darray_create(VkVertexInputBindingDescription);

    return config;
}

void pipeline_config_destroy(GraphicsPipelineConfiguration *config) {
    if (config->vertex_attribute_descriptions) {
        darray_destroy(config->vertex_attribute_descriptions);
        config->vertex_attribute_descriptions = NULL;
    }

    if (config->vertex_binding_descriptions) {
        darray_destroy(config->vertex_binding_descriptions);
        config->vertex_binding_descriptions = NULL;
    }

    memory_free(config);
}

void pipeline_config_add_va_description(GraphicsPipelineConfiguration *config, VkFormat format, u32 binding,
                                        u32 location, u32 offset) {
    VkVertexInputAttributeDescription va_desc = {0};
    va_desc.format = format;
    va_desc.binding = binding;
    va_desc.location = location;
    va_desc.offset = offset;
    darray_push(config->vertex_attribute_descriptions, va_desc);
}

void pipeline_config_add_va_binding(GraphicsPipelineConfiguration *config, VkVertexInputRate input_rate,
                                    u32 binding, u32 stride) {
    VkVertexInputBindingDescription binding_desc = {0};
    binding_desc.inputRate = input_rate;
    binding_desc.binding = binding;
    binding_desc.stride = stride;
    darray_push(config->vertex_binding_descriptions, binding_desc);
}