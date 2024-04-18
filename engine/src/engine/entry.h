#pragma once

#include <std/std.h>
#include "application.h"
#include "game_types.h"

extern bool create_game(Game *out_game);

int main(void) {
    Game game_instance;
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
    return 0;
}
