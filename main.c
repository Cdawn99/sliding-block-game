#include "entity.h"

#include "raymath.h"
#include "raylib.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 540
#define FPS 60

#define ACCELERATION_MAGNITUDE 100.0f
#define FRICTION_COEFFICIENT 0.2f

#define PLAYER_WIDTH 50.0f
#define PLAYER_HEIGHT 50.0f
#define PLAYER_MAX_V 300.0f

void apply_boundary_conditions(Entity *e) {
    if ((e->velocity.x > 0 && e->sprite.x + e->sprite.width >= SCREEN_WIDTH)
        || (e->velocity.x < 0 && e->sprite.x <= 0)) {
        e->velocity.x = -e->velocity.x;
    }

    if ((e->velocity.y > 0 && e->sprite.y <= 0)
        || (e->velocity.y < 0 && e->sprite.y + e->sprite.height >= SCREEN_HEIGHT)) {
        e->velocity.y = -e->velocity.y;
    }
}

void move_player(Entity *p, float dt) {
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

    Vector2 friction = Vector2Scale(p->velocity, FRICTION_COEFFICIENT);

    Vector2 total_acceleration = Vector2Subtract(acceleration, friction);

    entity_apply_acceleration(p, total_acceleration, dt);

    p->sprite.x = p->position.x - p->sprite.width/2.0f;
    p->sprite.y = p->position.y - p->sprite.height/2.0f;

    apply_boundary_conditions(p);

    entity_update_position(p, dt);
}

int main(void) {
    srand(time(NULL));

    Entity player = {
        .position = {.x = SCREEN_WIDTH/2.0f, .y = SCREEN_HEIGHT/2.0f},
        .velocity = {0},
        .max_velocity = PLAYER_MAX_V,
        .score = 0,
        .sprite = {
            .width = PLAYER_WIDTH,
            .height = PLAYER_HEIGHT,
        },
    };

    struct { Rectangle sprite; bool exists; } coin = {
        .sprite = {
            .width = 10,
            .height = 20,
        },
        .exists = false,
    };

    char score_text[20] = "Score: 0";

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Sliding block");
    SetTargetFPS(FPS);
    const float dt = 1.0f/FPS;

    while (!WindowShouldClose()) {
        move_player(&player, dt);

        if (!coin.exists) {
            do {
                coin.sprite.x = rand() % (SCREEN_WIDTH - (int)coin.sprite.width);
                coin.sprite.y = rand() % (SCREEN_HEIGHT - (int)coin.sprite.height);
            } while (CheckCollisionRecs(coin.sprite, player.sprite));
            coin.exists = true;
        } else if (CheckCollisionRecs(coin.sprite, player.sprite)) {
            coin.exists = false;
            player.score++;
            sprintf(score_text, "Score: %d", player.score);
        }

        BeginDrawing();
            ClearBackground(RAYWHITE);

            DrawRectangleRec(player.sprite, PURPLE);

            if (coin.exists) {
                DrawRectangleRec(coin.sprite, GOLD);
            }

            DrawText(score_text, 10, 10, 20, BLACK);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
