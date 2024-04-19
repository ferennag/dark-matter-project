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

typedef struct PhysicalDevice {
    VkPhysicalDevice vk_device;
    VkPhysicalDeviceProperties properties;
} PhysicalDevice;

typedef enum QueueCapability {
    QUEUE_GRAPHICS,
    QUEUE_COMPUTE,
    QUEUE_TRANSFER,
    QUEUE_PRESENT,

    // This must be last
    QUEUE_CAPABILITIES_MAX_ENUM,
} QueueCapability;

typedef struct QueueFamily {
    u32 index;
    bool capabilities[QUEUE_CAPABILITIES_MAX_ENUM];
} QueueFamily;

typedef struct Device {
    VkDevice vk_device;
    QueueFamily *queue_families;
    VkExtensionProperties *available_extensions;
} Device;

typedef struct SwapChain {
    VkSwapchainKHR vk_swapchain;
    VkSurfaceCapabilitiesKHR surface_capabilities;
    VkSurfaceFormatKHR surface_format;
    VkImage *images;
    VkImageView *image_views;
} SwapChain;

typedef struct VulkanContext {
    VkAllocationCallbacks *allocation_callbacks;
    VkDebugUtilsMessengerEXT debug_utils_messenger;

    VulkanInstance instance;
    PhysicalDevice physical_device;
    VkSurfaceKHR surface;
    Device device;
    SwapChain swap_chain;
} VulkanContext;