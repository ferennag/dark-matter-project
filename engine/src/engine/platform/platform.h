#pragma once

#include "std/defines.h"

typedef struct PlatformState {
    void *internal_state;
} PlatformState;

bool platform_startup(PlatformState *platform_state, const char *app_name);

void platform_shutdown(PlatformState *platform_state);

bool platform_handle_events(PlatformState *platform_state);

void platform_sleep(u32 ms);