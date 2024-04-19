#include <std/std.h>
#include <engine/entry.h>
#include <engine/game_types.h>
#include <engine/core/event.h>
#include <engine/core/input.h>
#include <std/core/memory.h>

typedef struct GameState {
} GameState;

bool initialize(struct Game *game_instance) {
    LOG_INFO("Game initialization...");
    return true;
}

void update(struct Game *game_instance, f32 delta_time) {
}

void render(struct Game *game_instance, f32 delta_time) {
}

void on_resize(struct Game *game_instance, u32 width, u32 height) {
}

void on_event(EventType event_type, EventData data) {
    switch (event_type) {
        case EVENT_KEYUP:
            break;
        case EVENT_KEYDOWN:
            if (data.data_u32[0] == KEY_ESCAPE || data.data_u32[0] == KEY_Q) {
                application_quit();
            }
            break;
        default:
            break;
    }
}

void on_shutdown(struct Game *game_instance) {
    memory_free(game_instance->game_state);
    game_instance->game_state = NULL;
    LOG_INFO("Game shutdown complete.");
}

bool create_game(Game *out_game) {
    ApplicationConfig config = {
            .name = "Dark Matter Project"
    };

    GameState *state = memory_alloc(sizeof(GameState));

    out_game->config = config;
    out_game->render = render;
    out_game->update = update;
    out_game->initialize = initialize;
    out_game->on_resize = on_resize;
    out_game->on_shutdown = on_shutdown;
    out_game->game_state = state;

    event_subscribe(EVENT_KEYDOWN, on_event);
    event_subscribe(EVENT_KEYUP, on_event);

    return true;
}