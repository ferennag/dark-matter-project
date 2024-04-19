#include "command_buffer.h"

void command_buffer_create(VulkanContext *context, QueueFamily *family, VkCommandBuffer *out) {
    VkCommandBufferAllocateInfo allocate_info = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocate_info.commandPool = family->command_pool;
    allocate_info.commandBufferCount = 1;
    VK_CHECK(vkAllocateCommandBuffers(context->device.vk_device, &allocate_info, out));
}

void command_buffer_begin(VkCommandBuffer *buffer) {
    VkCommandBufferBeginInfo begin_info = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    VK_CHECK(vkBeginCommandBuffer(*buffer, &begin_info))
}

void command_buffer_end(VkCommandBuffer *buffer) {
    VK_CHECK(vkEndCommandBuffer(*buffer))
}