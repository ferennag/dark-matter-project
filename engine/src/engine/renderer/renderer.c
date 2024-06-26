#include "renderer.h"

#if RENDERER == VULKAN
#include "vulkan/vulkan_renderer.h"
#endif

static RendererBackend renderer_backend = {0};

bool renderer_init(PlatformState *platform_state, const char *app_name) {
#if RENDERER == VULKAN
    renderer_backend.initialize = vulkan_initialize;
    renderer_backend.shutdown = vulkan_shutdown;
    renderer_backend.resized = vulkan_surface_resized;
    renderer_backend.render = vulkan_render;
#endif

    return renderer_backend.initialize(&renderer_backend, platform_state, app_name);
}

void renderer_shutdown() {
    renderer_backend.shutdown(&renderer_backend);
}

void renderer_resized(u32 width, u32 height) {
    renderer_backend.resized(&renderer_backend, width, height);
}

void renderer_draw_frame(RenderPacket *packet) {
    renderer_backend.render(&renderer_backend, packet);
}