#pragma once

#include "std/defines.h"

typedef struct Game Game;

typedef struct ApplicationConfig {
    const char *name;
} ApplicationConfig;

bool application_startup(Game *game);

void application_shutdown();

void application_run();

void application_quit();