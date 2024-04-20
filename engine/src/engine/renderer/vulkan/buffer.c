#include "buffer.h"
#include <std/containers/darray.h>
#include <std/core/memory.h>
#include "command_buffer.h"
#include "device.h"

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


bool
buffer_create(VulkanContext *context, VkBufferUsageFlags usage, VkMemoryPropertyFlags memory_property_flags, u32 size,
              Buffer *out) {
    out->size = size;

    VkBufferCreateInfo create_info = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    create_info.size = out->size;
    create_info.usage = usage;
    create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VK_CHECK(vkCreateBuffer(context->device.vk_device, &create_info, context->allocation_callbacks, &out->vk_buffer))
    vkGetBufferMemoryRequirements(context->device.vk_device, out->vk_buffer, &out->memory_requirements);

    u32 type;
    if (!find_memory_type(context, out->memory_requirements.memoryTypeBits, memory_property_flags, &type)) {
        return false;
    }

    VkMemoryAllocateInfo allocate_info = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    allocate_info.allocationSize = out->memory_requirements.size;
    allocate_info.memoryTypeIndex = type;

    VK_CHECK(vkAllocateMemory(context->device.vk_device, &allocate_info, context->allocation_callbacks, &out->memory))

    vkBindBufferMemory(context->device.vk_device, out->vk_buffer, out->memory, 0);

    return true;
}

void buffer_destroy(VulkanContext *context, Buffer *buffer) {
    vkDestroyBuffer(context->device.vk_device, buffer->vk_buffer, context->allocation_callbacks);
    vkFreeMemory(context->device.vk_device, buffer->memory, context->allocation_callbacks);
}

void buffer_copy(VulkanContext *context, Buffer *buffer, void *in_data) {
    void *data;
    vkMapMemory(context->device.vk_device, buffer->memory, 0, buffer->size, 0, &data);
    memory_copy(data, in_data, buffer->size);
    vkUnmapMemory(context->device.vk_device, buffer->memory);
}

void buffer_to_buffer_copy(VulkanContext *context, Buffer *dst, Buffer *src) {
    QueueFamily *family = find_queue_family(context, QUEUE_TRANSFER);
    VkCommandBuffer command_buffer;
    command_buffer_create(context, family, &command_buffer);

    VkCommandBufferBeginInfo begin = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    begin.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    VK_CHECK(vkBeginCommandBuffer(command_buffer, &begin))

    VkBufferCopy copy_region = {0};
    copy_region.srcOffset = 0;
    copy_region.dstOffset = 0;
    copy_region.size = src->size;
    vkCmdCopyBuffer(command_buffer, src->vk_buffer, dst->vk_buffer, 1, &copy_region);

    VK_CHECK(vkEndCommandBuffer(command_buffer))

    VkSubmitInfo submit_info = {VK_STRUCTURE_TYPE_SUBMIT_INFO};
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer;
    VK_CHECK(vkQueueSubmit(context->transfer_queue.vk_queue, 1, &submit_info, VK_NULL_HANDLE))
    VK_CHECK(vkQueueWaitIdle(context->transfer_queue.vk_queue))
}

bool vertex_buffer_create(VulkanContext *context, Vertex *vertices, VertexBuffer *out) {
    u32 size = sizeof(Vertex) * darray_length(vertices);
    if (!buffer_create(context, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, size,
                       &out->staging_buffer)) {
        return false;
    }

    if (!buffer_create(context, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, size, &out->device_buffer)) {
        return false;
    }

    vertex_buffer_copy(context, out, vertices);

    return true;
}

void vertex_buffer_destroy(VulkanContext *context, VertexBuffer *buffer) {
    buffer_destroy(context, &buffer->staging_buffer);
    buffer_destroy(context, &buffer->device_buffer);
}

void vertex_buffer_copy(VulkanContext *context, VertexBuffer *buffer, Vertex *vertices) {
    buffer_copy(context, &buffer->staging_buffer, vertices);
    buffer_to_buffer_copy(context, &buffer->device_buffer, &buffer->staging_buffer);
}