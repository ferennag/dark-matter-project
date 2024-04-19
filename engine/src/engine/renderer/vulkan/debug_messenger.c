#include "debug_messenger.h"

#include <std/core/logger.h>

VkBool32 debug_log(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                   VkDebugUtilsMessageTypeFlagsEXT messageTypes,
                   const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                   void *pUserData) {

    switch (messageSeverity) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            LOG_TRACE(pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            LOG_INFO(pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            LOG_WARN(pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            LOG_ERROR(pCallbackData->pMessage);
            break;
        default:
            break;
    }

    return VK_FALSE;
}

bool debug_messenger_create(VulkanContext *context) {
    VkDebugUtilsMessengerCreateInfoEXT create_info = {VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT};
    create_info.messageType =
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
    create_info.messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    create_info.pUserData = NULL;
    create_info.pfnUserCallback = debug_log;

    PFN_vkCreateDebugUtilsMessengerEXT create_func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
            context->instance.vk_instance, "vkCreateDebugUtilsMessengerEXT");

    create_func(context->instance.vk_instance, &create_info, context->allocation_callbacks,
                &context->debug_utils_messenger);

    return true;
}

void debug_messenger_destroy(VulkanContext *context) {
    PFN_vkDestroyDebugUtilsMessengerEXT destroy_func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
            context->instance.vk_instance, "vkDestroyDebugUtilsMessengerEXT");
    destroy_func(context->instance.vk_instance, context->debug_utils_messenger, context->allocation_callbacks);
}