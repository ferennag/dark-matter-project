#include "swapchain.h"
#include <std/containers/darray.h>
#include "device.h"

void create_image_views(VulkanContext *context, SwapChain *swap_chain) {
    swap_chain->image_views = darray_reserve(VkImageView, darray_length(swap_chain->images));

    for (int i = 0; i < darray_length(swap_chain->images); ++i) {
        VkImageViewCreateInfo image_view_create_info = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
        image_view_create_info.format = context->surface.format.format;
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

void create_framebuffers(VulkanContext *context, GraphicsPipeline *pipeline, SwapChain *in_out_swapchain) {
    in_out_swapchain->framebuffers = darray_reserve(VkFramebuffer, darray_length(in_out_swapchain->image_views));

    for (int i = 0; i < darray_length(in_out_swapchain->image_views); ++i) {
        VkImageView attachments[] = {
                in_out_swapchain->image_views[i]
        };

        VkFramebufferCreateInfo create_info = {VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
        create_info.attachmentCount = sizeof(attachments) / sizeof(attachments[0]);
        create_info.pAttachments = attachments;
        create_info.renderPass = pipeline->render_pass;
        create_info.layers = 1;
        create_info.width = context->surface.capabilities.currentExtent.width;
        create_info.height = context->surface.capabilities.currentExtent.height;

        VK_CHECK(vkCreateFramebuffer(context->device.vk_device, &create_info, context->allocation_callbacks,
                                     &in_out_swapchain->framebuffers[i]))
    }
}

bool swapchain_create(VulkanContext *context, SwapChain *out_swapchain) {
    u32 min_image_count = context->surface.capabilities.minImageCount + 1;
    min_image_count =
            min_image_count < context->surface.capabilities.maxImageCount ? min_image_count
                                                                          : context->surface.capabilities.maxImageCount;

    QueueFamily *present_queue_family = find_queue_family(context, QUEUE_PRESENT);

    if (!present_queue_family) {
        LOG_ERROR("Present queue unavailable for swapchain");
        return false;
    }

    VkSwapchainCreateInfoKHR swapchain_create_info = {VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
    swapchain_create_info.surface = context->surface.vk_surface;
    swapchain_create_info.imageExtent = context->surface.capabilities.currentExtent;
    swapchain_create_info.imageArrayLayers = 1;
    swapchain_create_info.minImageCount = min_image_count;
    swapchain_create_info.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchain_create_info.imageFormat = context->surface.format.format;
    swapchain_create_info.imageColorSpace = context->surface.format.colorSpace;
    swapchain_create_info.presentMode = context->surface.present_mode;
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
    create_framebuffers(context, &context->graphics_pipeline, out_swapchain);

    return true;
}

void swapchain_destroy(VulkanContext *context, SwapChain *swapchain) {
    darray_destroy(swapchain->images)
    swapchain->images = NULL;

    for (int i = 0; i < darray_length(swapchain->image_views); ++i) {
        vkDestroyImageView(context->device.vk_device, swapchain->image_views[i], context->allocation_callbacks);
    }
    darray_destroy(swapchain->image_views)
    swapchain->image_views = NULL;

    for (int i = 0; i < darray_length(swapchain->framebuffers); ++i) {
        vkDestroyFramebuffer(context->device.vk_device, swapchain->framebuffers[i], context->allocation_callbacks);
    }
    darray_destroy(swapchain->framebuffers)
    swapchain->framebuffers = NULL;

    vkDestroySwapchainKHR(context->device.vk_device, swapchain->vk_swapchain, context->allocation_callbacks);
}