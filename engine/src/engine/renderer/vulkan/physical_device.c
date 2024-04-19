#include "physical_device.h"
#include <std/containers/darray.h>

PhysicalDevice *query_host_devices(VulkanContext *context) {
    u32 count = 0;
    VK_CHECK(vkEnumeratePhysicalDevices(context->instance.vk_instance, &count, NULL))
    VkPhysicalDevice *devices = darray_reserve(VkPhysicalDevice, count);
    VK_CHECK(vkEnumeratePhysicalDevices(context->instance.vk_instance, &count, devices))

    PhysicalDevice *result = darray_create(PhysicalDevice);

    for (int i = 0; i < count; ++i) {
        VkPhysicalDevice device = devices[i];
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(device, &properties);
        LOG_INFO("Found available physical device: %s", properties.deviceName);
        PhysicalDevice physical_device = {
                .vk_device = device,
                .properties = properties
        };
        darray_push(result, physical_device);
    }

    darray_destroy(devices)
    return result;
}

bool select_best_device(VulkanContext *context, PhysicalDevice *devices) {
    PhysicalDevice *discreet_gpu = NULL;
    PhysicalDevice *integrated_gpu = NULL;

    for (int i = 0; i < darray_length(devices); ++i) {
        if (devices[i].properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            discreet_gpu = &devices[i];
            continue;
        } else if (devices[i].properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
            integrated_gpu = &devices[i];
            continue;
        }
    }

    if (!discreet_gpu && !integrated_gpu) {
        return false;
    }

    if (discreet_gpu) {
        context->physical_device = *discreet_gpu;
    } else {
        context->physical_device = *integrated_gpu;
    }

    LOG_INFO("Selected physical device: %s", context->physical_device.properties.deviceName);

    return true;
}

bool physical_device_select_best(VulkanContext *context) {
    PhysicalDevice *host_devices = query_host_devices(context);

    bool success = select_best_device(context, host_devices);
    darray_destroy(host_devices);
    return success;
}

void physical_device_shutdown(VulkanContext *context) {
}