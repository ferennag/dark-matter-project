#include "platform.h"
#include "SDL2/SDL.h"
#include "std/core/logger.h"
#include "std/core/memory.h"

typedef struct InternalState {
    SDL_Window *window;
} InternalState;

bool platform_startup(PlatformState *platform_state, const char *app_name) {
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        LOG_ERROR("Failed to initialize SDL: %s", SDL_GetError());
        return false;
    }

    SDL_Window *window = SDL_CreateWindow(app_name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 760,
                                          SDL_WINDOW_SHOWN | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_RESIZABLE);

    if (!window) {
        LOG_ERROR("Failed to create SDL window: %s", window);
        return false;
    }

    InternalState *internal_state = memory_alloc(sizeof(InternalState));
    platform_state->internal_state = internal_state;
    internal_state->window = window;

    return true;
}

void platform_shutdown(PlatformState *platform_state) {
    InternalState *internal_state = (InternalState *) platform_state->internal_state;
    SDL_DestroyWindow(internal_state->window);
    SDL_Quit();
}

bool platform_handle_event(PlatformState *platform_state, SDL_Event *event) {
    // TODO proper event handling

    switch (event->type) {
        case SDL_QUIT:
            return false;
        case SDL_KEYDOWN:
            if (event->key.keysym.sym == SDLK_ESCAPE || event->key.keysym.sym == SDLK_q) {
                return false;
            }
            break;
    }

    return true;
}

bool platform_handle_events(PlatformState *platform_state) {
    SDL_Event event;

    bool should_continue_running = true;
    while (SDL_PollEvent(&event)) {
        should_continue_running &= platform_handle_event(platform_state, &event);
    }

    return should_continue_running;
}

void platform_sleep(u32 ms) {
    SDL_Delay(ms);
}