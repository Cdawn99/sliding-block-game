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
#define FRICTION_COEFFICIENT 0.3f

int main(void) {
    srand(time(NULL));

    Entity player = {
        .position = {.x = SCREEN_WIDTH/2.0f, .y = SCREEN_HEIGHT/2.0f},
        .velocity = {0},
        .max_velocity = 300.0f,
        .score = 0,
    };

    Rectangle player_sprite = {
        .width = 50.0f,
        .height = 50.0f,
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

        Vector2 friction = {
            .x = player.velocity.x * FRICTION_COEFFICIENT,
            .y = player.velocity.y * FRICTION_COEFFICIENT,
        };

        // Normalize diagonal friction to have the same magnitude
        // as horizontal/vertical friction.
        if (friction.x != 0 && friction.y != 0) {
            friction.x /= sqrt(2);
            friction.y /= sqrt(2);
        }

        Vector2 total_acceleration = Vector2Subtract(acceleration, friction);

        entity_apply_acceleration(&player, total_acceleration, dt);

        player_sprite.x = player.position.x - player_sprite.width/2.0f;
        player_sprite.y = player.position.y - player_sprite.height/2.0f;

        if ((player.velocity.x > 0 && player_sprite.x + player_sprite.width >= SCREEN_WIDTH)
            || (player.velocity.x < 0 && player_sprite.x <= 0)) {
            player.velocity.x = -player.velocity.x;
        }

        if ((player.velocity.y > 0 && player_sprite.y <= 0)
            || (player.velocity.y < 0 && player_sprite.y + player_sprite.height >= SCREEN_HEIGHT)) {
            player.velocity.y = -player.velocity.y;
        }

        entity_update_position(&player, dt);

        if (!coin.exists) {
            do {
                coin.sprite.x = rand() % (SCREEN_WIDTH - (int)coin.sprite.width);
                coin.sprite.y = rand() % (SCREEN_HEIGHT - (int)coin.sprite.height);
            } while (CheckCollisionRecs(coin.sprite, player_sprite));
            coin.exists = true;
        } else if (CheckCollisionRecs(coin.sprite, player_sprite)) {
            coin.exists = false;
            player.score++;
            sprintf(score_text, "Score: %d", player.score);
        }

        BeginDrawing();
            ClearBackground(RAYWHITE);

            DrawRectangleRec(player_sprite, PURPLE);

            if (coin.exists) {
                DrawRectangleRec(coin.sprite, GOLD);
            }

            DrawText(score_text, 10, 10, 20, BLACK);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
