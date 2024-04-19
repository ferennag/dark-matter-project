#pragma once

#include "vulkan_types.h"

bool swapchain_create(VulkanContext *context, SwapChain *out_swapchain);

void swapchain_destroy(VulkanContext *context, SwapChain *swapchain);