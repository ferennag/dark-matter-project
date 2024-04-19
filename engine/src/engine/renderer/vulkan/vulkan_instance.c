#include "vulkan_instance.h"
#include <string.h>
#include <std/containers/darray.h>

void query_available_extensions(VkExtensionProperties **out_extensions) {
    u32 count = 0;
    VK_CHECK(vkEnumerateInstanceExtensionProperties(NULL, &count, NULL))
    *out_extensions = darray_reserve(VkExtensionProperties, count);
    VK_CHECK(vkEnumerateInstanceExtensionProperties(NULL, &count, *out_extensions))

    for (int i = 0; i < count; ++i) {
        LOG_INFO("Found instance extension: %s", (*out_extensions)[i].extensionName);
    }
}

bool is_extension_available(VulkanInstance *instance, const char *name) {
    for (int i = 0; i < darray_length(instance->available_extensions); ++i) {
        VkExtensionProperties *properties = &instance->available_extensions[i];
        if (strcmp(properties->extensionName, name) == 0) {
            return true;
        }
    }

    return false;
}

void query_available_layers(VkLayerProperties **out_layers) {
    u32 count = 0;
    VK_CHECK(vkEnumerateInstanceLayerProperties(&count, NULL))
    *out_layers = darray_reserve(VkLayerProperties, count);
    VK_CHECK(vkEnumerateInstanceLayerProperties(&count, *out_layers))

    for (int i = 0; i < count; ++i) {
        LOG_INFO("Found instance layer: %s", (*out_layers)[i].layerName);
    }
}

bool is_layer_available(VulkanInstance *instance, const char *name) {
    for (int i = 0; i < darray_length(instance->available_layers); ++i) {
        VkLayerProperties *properties = &instance->available_layers[i];
        if (strcmp(properties->layerName, name) == 0) {
            return true;
        }
    }

    return false;
}

bool vulkan_instance_create(VulkanContext *context, PlatformState *platform_state, const char *app_name) {
    VulkanInstance instance = {0};

    u32 api_version = 0;
    VK_CHECK(vkEnumerateInstanceVersion(&api_version))
    LOG_INFO("Vulkan version: %d.%d.%d.%d", VK_VERSION_MAJOR(api_version), VK_VERSION_MINOR(api_version),
             VK_VERSION_PATCH(api_version), VK_API_VERSION_VARIANT(api_version));

    VkApplicationInfo application_info = {VK_STRUCTURE_TYPE_APPLICATION_INFO};
    application_info.pEngineName = "Dark Matter Engine";
    application_info.pApplicationName = app_name;
    application_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    application_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    application_info.apiVersion = api_version;

    const char **extensions = 0;
    platform_required_vulkan_extensions(platform_state, &extensions);
    darray_push(extensions, &VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME)

    query_available_extensions(&instance.available_extensions);
    query_available_layers(&instance.available_layers);

    const char *requested_extensions[] = {
            "VK_EXT_debug_utils",
    };

    for (int i = 0; i < sizeof(requested_extensions) / sizeof(requested_extensions[0]); ++i) {
        if (is_extension_available(&instance, requested_extensions[i])) {
            darray_push(extensions, requested_extensions[i])
        }
    }

    const char *requested_layers[] = {
            "VK_LAYER_KHRONOS_validation",
            "VK_LAYER_KHRONOS_profiles",
    };
    const char **layers = darray_create(const char *);
    for (int i = 0; i < sizeof(requested_layers) / sizeof(requested_layers[0]); ++i) {
        if (is_layer_available(&instance, requested_layers[i])) {
            darray_push(layers, requested_layers[i])
        }
    }

    VkInstanceCreateInfo create_info = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
    create_info.pApplicationInfo = &application_info;
    create_info.ppEnabledExtensionNames = extensions;
    create_info.enabledExtensionCount = darray_length(extensions);
    create_info.ppEnabledLayerNames = layers;
    create_info.enabledLayerCount = darray_length(layers);
    create_info.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

    VK_CHECK(vkCreateInstance(&create_info, context->allocation_callbacks, &instance.vk_instance))

    darray_destroy(extensions)
    darray_destroy(layers)

    context->instance = instance;

    return true;
}

void vulkan_instance_destroy(VulkanContext *context) {
    darray_destroy(context->instance.available_extensions);
    context->instance.available_extensions = NULL;

    darray_destroy(context->instance.available_layers);
    context->instance.available_layers = NULL;

    vkDestroyInstance(context->instance.vk_instance, context->allocation_callbacks);
}