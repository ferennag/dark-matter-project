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
    VkCommandPool command_pool;
} QueueFamily;

typedef struct Queue {
    QueueFamily *family;
    VkQueue vk_queue;
    VkCommandBuffer command_buffer;
} Queue;

typedef struct Device {
    VkDevice vk_device;
    QueueFamily *queue_families;
    VkExtensionProperties *available_extensions;
} Device;

typedef struct Surface {
    VkSurfaceKHR vk_surface;
    VkSurfaceFormatKHR format;
    VkPresentModeKHR present_mode;
    VkSurfaceCapabilitiesKHR capabilities;
} Surface;

typedef struct SwapChain {
    VkSwapchainKHR vk_swapchain;
    VkImage *images;
    VkImageView *image_views;
    VkFramebuffer *framebuffers;
} SwapChain;

typedef struct GraphicsPipelineConfiguration {
    const char *vertex_shader;
    const char *fragment_shader;

    VkVertexInputAttributeDescription *vertex_attribute_descriptions;
    VkVertexInputBindingDescription *vertex_binding_descriptions;
} GraphicsPipelineConfiguration;

typedef struct GraphicsPipeline {
    VkRenderPass render_pass;
    VkShaderModule vertex_shader_module;
    VkShaderModule fragment_shader_module;
    VkPipelineLayout pipeline_layout;
    VkPipeline vk_pipeline;
} GraphicsPipeline;

typedef struct Buffer {
    VkBuffer vk_buffer;
    u32 size;
    VkMemoryRequirements memory_requirements;
    VkDeviceMemory memory;
} Buffer;

typedef struct VertexBuffer {
    Buffer staging_buffer;
    Buffer device_buffer;
} VertexBuffer;

// TODO remove this
typedef struct Scene Scene;

typedef struct VulkanContext {
    VkAllocationCallbacks *allocation_callbacks;
    VkDebugUtilsMessengerEXT debug_utils_messenger;

    VulkanInstance instance;
    PhysicalDevice physical_device;
    Device device;
    Surface surface;
    SwapChain swap_chain;
    GraphicsPipeline graphics_pipeline;

    Queue graphics_queue;
    Queue present_queue;
    Queue transfer_queue;

    VkSemaphore image_available_semaphore;
    VkSemaphore render_finished_semaphore;
    VkFence in_flight_fence;
    Scene *scene;
} VulkanContext;