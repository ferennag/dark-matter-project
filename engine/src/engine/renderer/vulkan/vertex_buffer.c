#include "vertex_buffer.h"
#include <std/containers/darray.h>
#include <std/core/memory.h>

bool find_memory_type(VulkanContext *context, uint32_t type_filter, VkMemoryPropertyFlags properties, u32 *result) {
    VkPhysicalDeviceMemoryProperties mem_properties;
    vkGetPhysicalDeviceMemoryProperties(context->physical_device.vk_device, &mem_properties);

    for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++) {
        if ((type_filter & (1 << i)) && (mem_properties.memoryTypes[i].propertyFlags & properties) == properties) {
            *result = i;
            return true;
        }
    }

    return false;
}


bool vertex_buffer_create(VulkanContext *context, Vertex *vertices, VertexBuffer *out) {
    out->size = darray_length(vertices) * sizeof(Vertex);

    VkBufferCreateInfo create_info = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    create_info.size = out->size;
    create_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VK_CHECK(vkCreateBuffer(context->device.vk_device, &create_info, context->allocation_callbacks, &out->buffer))
    vkGetBufferMemoryRequirements(context->device.vk_device, out->buffer, &out->memory_requirements);

    u32 type;
    if (!find_memory_type(context, out->memory_requirements.memoryTypeBits,
                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &type)) {
        return false;
    }

    VkMemoryAllocateInfo allocate_info = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    allocate_info.allocationSize = out->memory_requirements.size;
    allocate_info.memoryTypeIndex = type;

    VK_CHECK(vkAllocateMemory(context->device.vk_device, &allocate_info, context->allocation_callbacks, &out->memory))

    vkBindBufferMemory(context->device.vk_device, out->buffer, out->memory, 0);

    return true;
}

void vertex_buffer_destroy(VulkanContext *context, VertexBuffer *buffer) {
    vkDestroyBuffer(context->device.vk_device, buffer->buffer, context->allocation_callbacks);
    vkFreeMemory(context->device.vk_device, buffer->memory, context->allocation_callbacks);
}

void vertex_buffer_copy(VulkanContext *context, VertexBuffer *buffer, Vertex *vertices) {
    void *data;
    vkMapMemory(context->device.vk_device, buffer->memory, 0, buffer->size, 0, &data);
    memory_copy(data, vertices, buffer->size);
    vkUnmapMemory(context->device.vk_device, buffer->memory);
}