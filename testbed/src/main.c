#include <std/std.h>
#include <engine/entry.h>
#include <engine/game_types.h>
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

    return true;
}