#include "platform.h"
#include "SDL2/SDL.h"
#include "std/core/logger.h"
#include "std/core/memory.h"
#include "engine/core/event.h"
#include "engine/core/input.h"

typedef struct InternalState {
    SDL_Window *window;
} InternalState;

EventData platform_translate_key_event(SDL_KeyboardEvent *event);

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
    switch (event->type) {
        case SDL_QUIT:
            event_dispatch(EVENT_QUIT, (EventData) {0});
            return false;
        case SDL_KEYDOWN: {
            EventData data = platform_translate_key_event(&event->key);
            event_dispatch(EVENT_KEYDOWN, data);
            break;
        }
        case SDL_KEYUP: {
            EventData data = platform_translate_key_event(&event->key);
            event_dispatch(EVENT_KEYUP, data);
            break;
        }
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

EventData platform_translate_key_event(SDL_KeyboardEvent *event) {
    EventData data = {0};
    switch (event->keysym.sym) {
        case SDLK_ESCAPE:
            data.data_u32[0] = KEY_ESCAPE;
            break;
        case SDLK_SPACE:
            data.data_u32[0] = KEY_SPACE;
            break;
        case SDLK_a:
            data.data_u32[0] = KEY_A;
            break;
        case SDLK_b:
            data.data_u32[0] = KEY_B;
            break;
        case SDLK_c:
            data.data_u32[0] = KEY_C;
            break;
        case SDLK_d:
            data.data_u32[0] = KEY_D;
            break;
        case SDLK_e:
            data.data_u32[0] = KEY_E;
            break;
        case SDLK_f:
            data.data_u32[0] = KEY_F;
            break;
        case SDLK_g:
            data.data_u32[0] = KEY_G;
            break;
        case SDLK_h:
            data.data_u32[0] = KEY_H;
            break;
        case SDLK_i:
            data.data_u32[0] = KEY_I;
            break;
        case SDLK_j:
            data.data_u32[0] = KEY_J;
            break;
        case SDLK_k:
            data.data_u32[0] = KEY_K;
            break;
        case SDLK_l:
            data.data_u32[0] = KEY_L;
            break;
        case SDLK_m:
            data.data_u32[0] = KEY_M;
            break;
        case SDLK_n:
            data.data_u32[0] = KEY_N;
            break;
        case SDLK_o:
            data.data_u32[0] = KEY_O;
            break;
        case SDLK_p:
            data.data_u32[0] = KEY_P;
            break;
        case SDLK_q:
            data.data_u32[0] = KEY_Q;
            break;
        case SDLK_r:
            data.data_u32[0] = KEY_R;
            break;
        case SDLK_s:
            data.data_u32[0] = KEY_S;
            break;
        case SDLK_t:
            data.data_u32[0] = KEY_T;
            break;
        case SDLK_u:
            data.data_u32[0] = KEY_U;
            break;
        case SDLK_v:
            data.data_u32[0] = KEY_V;
            break;
        case SDLK_w:
            data.data_u32[0] = KEY_W;
            break;
        case SDLK_x:
            data.data_u32[0] = KEY_X;
            break;
        case SDLK_y:
            data.data_u32[0] = KEY_Y;
            break;
        case SDLK_z:
            data.data_u32[0] = KEY_Z;
            break;
    }
    return data;
}