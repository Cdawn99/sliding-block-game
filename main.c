#include "entity.h"

#include <raylib.h>
#include <raymath.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 540
#define FPS 60

#define ACCELERATION_MAGNITUDE 100.0f
#define FRICTION_COEFFICIENT 0.2f

#define ENTITY_WIDTH 50.0f
#define ENTITY_HEIGHT 50.0f

#define PLAYER_MAX_V 300.0f

typedef struct Coin {
    Rectangle sprite;
    bool exists;
    Color color;
} Coin;

char score_text[20] = "Score: 0";

/**
 * Generate a random float between 0.0f and 1.0f (inclusive).
 */
static inline float rand_float(void) {
    return (float)rand()/RAND_MAX;
}

static Vector2 get_player_acceleration(Entity *player) {
    Vector2 acceleration = {
        .x = (IsKeyDown(KEY_RIGHT) - IsKeyDown(KEY_LEFT)) * ACCELERATION_MAGNITUDE,
        .y = (IsKeyDown(KEY_UP) - IsKeyDown(KEY_DOWN)) * ACCELERATION_MAGNITUDE,
    };

    // Normalize diagonal acceleration to have the same magnitude
    // as horizontal/vertical acceleration.
    if (acceleration.x != 0 && acceleration.y != 0) {
        acceleration.x /= sqrt(2);
        acceleration.y /= sqrt(2);
    }

    Vector2 friction = Vector2Scale(player->velocity, FRICTION_COEFFICIENT);

    return Vector2Subtract(acceleration, friction);
}

static Vector2 get_enemy_acceleration(Entity *enemy) {
    static int tick = 0;
    static const int max_tick = 300;

    // Relative to the screen's center.
    Vector2 pos = Vector2Subtract((Vector2){.x = SCREEN_WIDTH/2.0f, .y = SCREEN_HEIGHT/2.0f}, enemy->position);
    pos = Vector2Scale(Vector2Normalize(pos), ACCELERATION_MAGNITUDE);

    Vector2 result = Vector2Rotate(pos, 2.0f * PI * tick++ / max_tick);

    tick = (tick + 1) % max_tick;

    return result;
}

static bool gameplay_loop(Entity *player, Entity *enemy, Coin *coin) {
    const float dt = GetFrameTime();
    bool collided = entity_elastic_collision(player, enemy);

    if (collided) {
        if (player->score > 0) {
            player->score--;
            sprintf(score_text, "Score: %d", player->score);
        } else {
            return false;
        }
    }

    entity_move(player, get_player_acceleration(player), dt, SCREEN_WIDTH, SCREEN_HEIGHT);
    entity_move(enemy, get_enemy_acceleration(enemy), dt, SCREEN_WIDTH, SCREEN_HEIGHT);

    if (!coin->exists) {
        do {
            coin->sprite.x = rand() % (SCREEN_WIDTH - (int)coin->sprite.width);
            coin->sprite.y = rand() % (SCREEN_HEIGHT - (int)coin->sprite.height);
        } while (CheckCollisionRecs(coin->sprite, player->sprite));
        coin->exists = true;
    } else if (CheckCollisionRecs(coin->sprite, player->sprite)) {
        coin->exists = false;
        player->score++;
        sprintf(score_text, "Score: %d", player->score);
    }

    BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawRectangleRec(player->sprite, player->color);
        DrawRectangleRec(enemy->sprite, enemy->color);

        if (coin->exists) {
            DrawRectangleRec(coin->sprite, coin->color);
        }

        DrawText(score_text, 10, 10, 20, BLACK);
    EndDrawing();

    return true;
}

int main(void) {
    srand(time(NULL));

    Entity player = {
        .position = {.x = SCREEN_WIDTH/2.0f, .y = 2.0f*SCREEN_HEIGHT/3.0f},
        .velocity = {0},
        .max_velocity = PLAYER_MAX_V,
        .score = 0,
        .sprite = {
            .width = ENTITY_WIDTH,
            .height = ENTITY_HEIGHT,
        },
        .color = PURPLE,
    };

    Entity enemy = {
        .position = {.x = SCREEN_WIDTH/2.0f, .y = SCREEN_HEIGHT/3.0f},
        .velocity = {.x = rand_float()*PLAYER_MAX_V/2.0f, .y = rand_float()*PLAYER_MAX_V/2.0f},
        .max_velocity = PLAYER_MAX_V,
        .score = 1,
        .sprite = {
            .width = ENTITY_WIDTH,
            .height = ENTITY_HEIGHT,
        },
        .color = RED,
    };

    Coin coin = {
        .sprite = {
            .width = 10,
            .height = 20,
        },
        .color = GOLD,
        .exists = false,
    };

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Sliding block");
    SetTargetFPS(FPS);

    bool game_continue = true;
    while (!WindowShouldClose() && game_continue) {
        game_continue = gameplay_loop(&player, &enemy, &coin);
    }

    CloseWindow();
    return 0;
}
