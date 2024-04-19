#pragma once

#include <stdlib.h>
#include <std/core/logger.h>
#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>

#define RENDERER VULKAN

#define VK_CHECK(expr) {                                            \
    VkResult _result = expr;                                        \
    if(_result != VK_SUCCESS) {                                     \
        LOG_ERROR("Vulkan error: %s", string_VkResult(_result));    \
        exit(-2);                                                   \
    }                                                               \
}

typedef struct VulkanInstance {
    VkInstance vk_instance;
    VkExtensionProperties *available_extensions;
    VkLayerProperties *available_layers;
} VulkanInstance;

typedef struct VulkanContext {
    VkAllocationCallbacks *allocation_callbacks;
    VulkanInstance instance;
    VkPhysicalDevice physical_device;
    VkDevice device;
} VulkanContext;