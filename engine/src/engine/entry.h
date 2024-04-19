#pragma once

#include <std/std.h>
#include "application.h"
#include "game_types.h"
#include "engine/core/event.h"

extern bool create_game(Game *out_game);

int main(void) {
    Game game_instance;

    init_logging();
    event_init();

    if (!create_game(&game_instance)) {
        LOG_ERROR("Failed to create game instance!");
        return -1;
    }

    if (!application_startup(&game_instance)) {
        LOG_ERROR("Failed to initialize application.");
        return -1;
    }

    application_run();
    application_shutdown();

    event_shutdown();
    shutdown_logging();

    return 0;
}
