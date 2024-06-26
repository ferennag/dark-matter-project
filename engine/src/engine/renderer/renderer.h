#pragma once

#include <std/defines.h>
#include "engine/platform/platform.h"

typedef struct RenderPacket {} RenderPacket;

typedef struct RendererBackend {
    bool (*initialize)(struct RendererBackend *backend, PlatformState *platform_state, const char *app_name);

    void (*shutdown)(struct RendererBackend *backend);

    void (*resized)(struct RendererBackend *backend, u32 width, u32 height);

    void (*render)(struct RendererBackend *backend, RenderPacket *packet);

    void *renderer_context;
} RendererBackend;

bool renderer_init(PlatformState *platform_state, const char *app_name);

void renderer_shutdown();

void renderer_resized(u32 width, u32 height);

void renderer_draw_frame(RenderPacket *packet);
