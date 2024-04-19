#include "surface.h"
#include <std/containers/darray.h>

bool select_surface_format(VulkanContext *context, VkSurfaceFormatKHR *out_format) {
    u32 count = 0;
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(context->physical_device.vk_device, context->surface.vk_surface,
                                                  &count, NULL))
    VkSurfaceFormatKHR *surface_formats = darray_reserve(VkSurfaceFormatKHR, count);
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(context->physical_device.vk_device, context->surface.vk_surface,
                                                  &count,
                                                  surface_formats))

    bool found = false;
    for (int i = 0; i < count; ++i) {
        VkSurfaceFormatKHR format = surface_formats[i];
        if (format.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR && format.format == VK_FORMAT_B8G8R8A8_SRGB) {
            *out_format = surface_formats[i];
            found = true;
            break;
        }
    }

    darray_destroy(surface_formats);
    return found;
}

bool select_present_mode(VulkanContext *context, VkPresentModeKHR *out_present_mode) {
    u32 count = 0;
    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(context->physical_device.vk_device, context->surface.vk_surface,
                                                       &count,
                                                       NULL))

    if (count == 0) {
        return false;
    }

    VkPresentModeKHR *present_modes = darray_reserve(VkPresentModeKHR, count);
    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(context->physical_device.vk_device, context->surface.vk_surface,
                                                       &count,
                                                       NULL))

    for (int i = 0; i < count; ++i) {
        if (present_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
            *out_present_mode = present_modes[i];
            return true;
        }
    }

    *out_present_mode = present_modes[0];
    return true;
}

bool surface_create(PlatformState *platform_state, VulkanContext *context) {
    if (!platform_vulkan_create_surface(platform_state, context, &context->surface.vk_surface)) {
        return false;
    }

    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(context->physical_device.vk_device, context->surface.vk_surface,
                                                       &context->surface.capabilities))

    select_surface_format(context, &context->surface.format);
    select_present_mode(context, &context->surface.present_mode);

    return true;
}

void surface_destroy(VulkanContext *context) {
    vkDestroySurfaceKHR(context->instance.vk_instance, context->surface.vk_surface, context->allocation_callbacks);
}