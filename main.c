#include "raylib.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 540
#define FPS 60

#define DIRECTIONAL_VELOCITY 200.0f

int main(void) {
    Rectangle rec = {
        .width = 50.0f,
        .height = 50.0f,
    };
    rec.x = (SCREEN_WIDTH - rec.width)/2.0f;
    rec.y = (SCREEN_HEIGHT - rec.height)/2.0f;

    Vector2 velocity = {0};

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Sliding block");
    SetTargetFPS(FPS);
    const float delta_t = 1.0f/FPS;

    while (!WindowShouldClose()) {
        velocity.x = (IsKeyDown(KEY_RIGHT) - IsKeyDown(KEY_LEFT)) * DIRECTIONAL_VELOCITY * delta_t;
        velocity.y = (IsKeyDown(KEY_DOWN) - IsKeyDown(KEY_UP)) * DIRECTIONAL_VELOCITY * delta_t;

        if (velocity.x > 0 && rec.x + rec.width <= SCREEN_WIDTH) {
            rec.x += velocity.x;
        } else if (velocity.x < 0 && rec.x >= 0) {
            rec.x += velocity.x;
        }

        if (velocity.y < 0 && rec.y >= 0) {
            rec.y += velocity.y;
        } else if (velocity.y > 0 && rec.y + rec.height <= SCREEN_HEIGHT) {
            rec.y += velocity.y;
        }

        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawRectangleRec(rec, PURPLE);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

