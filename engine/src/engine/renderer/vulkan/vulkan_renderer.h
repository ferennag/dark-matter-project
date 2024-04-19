#pragma once

#include <std/std.h>
#include "vulkan_types.h"
#include "engine/renderer/renderer.h"

bool vulkan_initialize(RendererBackend *backend, PlatformState *platform_state, const char *app_name);

void vulkan_shutdown(RendererBackend *backend);

void vulkan_surface_resized(RendererBackend *backend, u32 width, u32 height);

void vulkan_render(RendererBackend *backend, RenderPacket *packet);