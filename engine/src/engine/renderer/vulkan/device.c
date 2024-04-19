#include "device.h"
#include <string.h>
#include <std/containers/darray.h>

QueueFamily *query_queue_families(VulkanContext *context) {
    u32 count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(context->physical_device.vk_device, &count, NULL);
    VkQueueFamilyProperties *properties = darray_reserve(VkQueueFamilyProperties, count);
    vkGetPhysicalDeviceQueueFamilyProperties(context->physical_device.vk_device, &count, properties);

    QueueFamily *families = darray_reserve(QueueFamily, count);
    for (int i = 0; i < count; ++i) {
        QueueFamily family = {0};
        family.index = i;

        family.capabilities[QUEUE_GRAPHICS] = properties->queueFlags & VK_QUEUE_GRAPHICS_BIT;
        family.capabilities[QUEUE_COMPUTE] = properties->queueFlags & VK_QUEUE_COMPUTE_BIT;
        family.capabilities[QUEUE_TRANSFER] = properties->queueFlags & VK_QUEUE_TRANSFER_BIT;

        VkBool32 supported = VK_FALSE;
        VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(context->physical_device.vk_device, i, context->surface,
                                                      &supported));
        family.capabilities[QUEUE_PRESENT] = supported == VK_TRUE;

        families[i] = family;
    }

    darray_destroy(properties);
    return families;
}

VkDeviceQueueCreateInfo *create_queue_create_infos(QueueFamily *queue_families) {
    float priority = 1.0f;
    VkDeviceQueueCreateInfo *result = darray_create(VkDeviceQueueCreateInfo);
    for (int i = 0; i < darray_length(queue_families); ++i) {
        VkDeviceQueueCreateInfo queue_create_info = {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
        queue_create_info.queueCount = 1;
        queue_create_info.queueFamilyIndex = queue_families[i].index;
        queue_create_info.pQueuePriorities = &priority;
        darray_push(result, queue_create_info);
    }
    return result;
}

VkExtensionProperties *query_available_device_extensions(VulkanContext *context) {
    u32 count = 0;
    VK_CHECK(vkEnumerateDeviceExtensionProperties(context->physical_device.vk_device, NULL, &count, NULL))
    VkExtensionProperties *extensions = darray_reserve(VkExtensionProperties, count);
    VK_CHECK(vkEnumerateDeviceExtensionProperties(context->physical_device.vk_device, NULL, &count, extensions))

    for (int i = 0; i < count; ++i) {
        LOG_INFO("Found device extension: %s", extensions[i].extensionName);
    }

    return extensions;
}

bool is_device_extension_available(Device *device, const char *name) {
    for (int i = 0; i < darray_length(device->available_extensions); ++i) {
        if (strcmp(device->available_extensions[i].extensionName, name) == 0) {
            return true;
        }
    }
    return false;
}

bool device_create(VulkanContext *context) {
    Device device = {0};
    device.queue_families = query_queue_families(context);
    device.available_extensions = query_available_device_extensions(context);

    VkDeviceQueueCreateInfo *queue_create_infos = create_queue_create_infos(device.queue_families);

    const char **extensions = darray_create(const char *);
    if (is_device_extension_available(&device, "VK_KHR_portability_subset")) {
        darray_push(extensions, &"VK_KHR_portability_subset");
    }

    VkDeviceCreateInfo device_create_info = {VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
    device_create_info.enabledExtensionCount = darray_length(extensions);
    device_create_info.ppEnabledExtensionNames = extensions;
    device_create_info.pQueueCreateInfos = queue_create_infos;
    device_create_info.queueCreateInfoCount = darray_length(queue_create_infos);

    VK_CHECK(vkCreateDevice(context->physical_device.vk_device, &device_create_info, context->allocation_callbacks,
                            &device.vk_device))

    context->device = device;
    darray_destroy(extensions)
    darray_destroy(queue_create_infos)
    return true;
}

void device_destroy(VulkanContext *context) {
    darray_destroy(context->device.queue_families);
    context->device.queue_families = NULL;

    darray_destroy(context->device.available_extensions);
    context->device.available_extensions = NULL;

    vkDestroyDevice(context->device.vk_device, context->allocation_callbacks);
}