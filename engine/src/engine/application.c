#include "application.h"
#include "std/core/memory.h"
#include "std/core/logger.h"
#include "engine/platform/platform.h"
#include "game_types.h"
#include "renderer/renderer.h"

typedef struct ApplicationState {
    bool is_running;
    bool is_suspended;
    PlatformState *platform_state;
    Game *game;
} ApplicationState;

static ApplicationState application_state = {0};

bool application_startup(Game *game) {
    application_state.game = game;

    application_state.platform_state = memory_alloc(sizeof(PlatformState));
    if (!platform_startup(application_state.platform_state, game->config.name)) {
        LOG_FATAL("Failed to initialize platform!");
        return false;
    }

    if(!renderer_init(application_state.platform_state, game->config.name)) {
        LOG_FATAL("Failed to initialize the renderer!");
        return false;
    }

    if (!application_state.game->initialize(application_state.game)) {
        LOG_ERROR("Failed to initialize the game instance");
        return false;
    }

    return true;
}

void application_shutdown() {
    application_state.game->on_shutdown(application_state.game);
    renderer_shutdown();
    platform_shutdown(application_state.platform_state);
}

void application_run() {
    application_state.is_running = true;

    while (application_state.is_running) {
        if (!platform_handle_events(application_state.platform_state)) {
            application_state.is_running = false;
        }

        application_state.game->update(application_state.game, 0);
        application_state.game->render(application_state.game, 0);
        renderer_draw_frame(NULL);
    }
}

void application_quit() {
    application_state.is_running = false;
}