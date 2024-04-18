#pragma once

#include <std/std.h>
#include "application.h"

typedef struct Game {
    ApplicationConfig config;

    bool (*initialize)(struct Game *game_instance);

    void (*update)(struct Game *game_instance, f32 delta_time);

    void (*render)(struct Game *game_instance, f32 delta_time);

    void (*on_resize)(struct Game *game_instance, u32 width, u32 height);

    void (*on_shutdown)(struct Game *game_instance);

    void *game_state;
} Game;