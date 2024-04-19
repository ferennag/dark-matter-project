#pragma once

#include "std/defines.h"

typedef struct Game Game;

typedef struct ApplicationConfig {
    const char *name;
} ApplicationConfig;

API bool application_startup(Game *game);

API void application_shutdown();

API void application_run();

API void application_quit();
