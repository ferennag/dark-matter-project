#include "swapchain.h"
#include <std/containers/darray.h>
#include "device.h"

bool select_surface_format(VulkanContext *context, VkSurfaceFormatKHR *out_format) {
    u32 count = 0;
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(context->physical_device.vk_device, context->surface, &count, NULL))
    VkSurfaceFormatKHR *surface_formats = darray_reserve(VkSurfaceFormatKHR, count);
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(context->physical_device.vk_device, context->surface, &count,
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
    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(context->physical_device.vk_device, context->surface, &count,
                                                       NULL))

    if (count == 0) {
        return false;
    }

    VkPresentModeKHR *present_modes = darray_reserve(VkPresentModeKHR, count);
    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(context->physical_device.vk_device, context->surface, &count,
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

void create_image_views(VulkanContext *context, SwapChain *swap_chain) {
    swap_chain->image_views = darray_reserve(VkImageView, darray_length(swap_chain->images));

    for (int i = 0; i < darray_length(swap_chain->images); ++i) {
        VkImageViewCreateInfo image_view_create_info = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
        image_view_create_info.format = swap_chain->surface_format.format;
        image_view_create_info.image = swap_chain->images[i];
        image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        image_view_create_info.subresourceRange.levelCount = 1;
        image_view_create_info.subresourceRange.layerCount = 1;
        image_view_create_info.subresourceRange.baseArrayLayer = 0;
        image_view_create_info.subresourceRange.baseMipLevel = 0;

        VK_CHECK(vkCreateImageView(context->device.vk_device, &image_view_create_info, context->allocation_callbacks,
                                   &swap_chain->image_views[i]))
    }
}

bool swapchain_create(VulkanContext *context, SwapChain *out_swapchain) {
    VkSurfaceCapabilitiesKHR surface_capabilities;
    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(context->physical_device.vk_device, context->surface,
                                                       &surface_capabilities))
    out_swapchain->surface_capabilities = surface_capabilities;

    u32 min_image_count = surface_capabilities.minImageCount + 1;
    min_image_count =
            min_image_count < surface_capabilities.maxImageCount ? min_image_count : surface_capabilities.maxImageCount;

    VkSurfaceFormatKHR surface_format;
    if (!select_surface_format(context, &surface_format)) {
        LOG_ERROR("Couldn't find a suitable surface format for the swapchain");
        return false;
    }
    out_swapchain->surface_format = surface_format;

    VkPresentModeKHR present_mode;
    if (!select_present_mode(context, &present_mode)) {
        LOG_ERROR("Couldn't find a suitable present mode for the swapchain");
        return false;
    }

    QueueFamily *present_queue_family = find_queue_family(context, QUEUE_PRESENT);

    if (!present_queue_family) {
        LOG_ERROR("Present queue unavailable for swapchain");
        return false;
    }

    VkSwapchainCreateInfoKHR swapchain_create_info = {VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
    swapchain_create_info.surface = context->surface;
    swapchain_create_info.imageExtent = surface_capabilities.currentExtent;
    swapchain_create_info.imageArrayLayers = 1;
    swapchain_create_info.minImageCount = min_image_count;
    swapchain_create_info.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchain_create_info.imageFormat = surface_format.format;
    swapchain_create_info.imageColorSpace = surface_format.colorSpace;
    swapchain_create_info.presentMode = present_mode;
    swapchain_create_info.pQueueFamilyIndices = &present_queue_family->index;
    swapchain_create_info.queueFamilyIndexCount = 1;

    VK_CHECK(vkCreateSwapchainKHR(context->device.vk_device, &swapchain_create_info, context->allocation_callbacks,
                                  &out_swapchain->vk_swapchain))

    u32 count = 0;
    VK_CHECK(vkGetSwapchainImagesKHR(context->device.vk_device, out_swapchain->vk_swapchain, &count, NULL))
    VkImage *images = darray_reserve(VkImage, count);
    VK_CHECK(vkGetSwapchainImagesKHR(context->device.vk_device, out_swapchain->vk_swapchain, &count, images))
    out_swapchain->images = images;

    create_image_views(context, out_swapchain);

    return true;
}

void swapchain_destroy(VulkanContext *context, SwapChain *swapchain) {
    vkDestroySwapchainKHR(context->device.vk_device, swapchain->vk_swapchain, context->allocation_callbacks);
    darray_destroy(swapchain->images)
    swapchain->images = NULL;

    for (int i = 0; i < darray_length(swapchain->image_views); ++i) {
        vkDestroyImageView(context->device.vk_device, swapchain->image_views[i], context->allocation_callbacks);
    }
    darray_destroy(swapchain->image_views)
    swapchain->image_views = NULL;
}