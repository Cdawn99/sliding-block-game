#include "entity.h"
#include "highscore.h"

#include <raylib.h>
#include <raymath.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 540
#define FPS 60

#define BOX_SPACING 10
#define EXTRA_BOX_SIZE 15
#define BOX_COLOR SKYBLUE
#define BOX_COLOR_HOVERED BLUE

#define ACCELERATION_MAGNITUDE 100.0f
#define FRICTION_COEFFICIENT 0.2f

#define ENTITY_WIDTH 50.0f
#define ENTITY_HEIGHT 50.0f

#define PLAYER_MAX_V 300.0f

#define ENEMY_MAX_SCORE 3

typedef enum GameState {
    GAME_START_MENU,
    GAME_PLAYING,
    GAME_HIGHSCORE,
    GAME_EXIT,
} GameState;

typedef struct Coin {
    Rectangle sprite;
    bool exists;
    Color color;
} Coin;

char score_text[12] = "Score: 0"; // 7 for 'Score: ' + 4 for the score (up to 9999) + 1 for null terminator.
char lives_text[9] = "Lives: 0";  // 7 for 'Lives: ' + 1 for lives (up to 9) + 1 for null terminator.

static void update_text(Entity *player) {
    sprintf(score_text, "Score: %d", player->score);
    sprintf(lives_text, "Lives: %d", player->lives);
}

static GameState start_menu(void) {
    Vector2 center = {
        .x = SCREEN_WIDTH/2.0f,
        .y = SCREEN_HEIGHT/2.0f
    };

    char start_text[] = "Start";
    Vector2 start_size = MeasureTextEx(GetFontDefault(), start_text, 40, 4);

    char hs_text[] = "High score";
    Vector2 hs_size = MeasureTextEx(GetFontDefault(), hs_text, 40, 4);

    char exit_text[] = "Exit";
    Vector2 exit_size = MeasureTextEx(GetFontDefault(), exit_text, 40, 4);

    // Since the 'High score' text is the largest, box size will be dictated by it.
    Vector2 pos = Vector2Subtract(center, Vector2Scale(hs_size, 1.0f/2.0f));

    Rectangle start_box = {
        .width = hs_size.x + EXTRA_BOX_SIZE,
        .height = hs_size.y + EXTRA_BOX_SIZE,
        .x = pos.x - EXTRA_BOX_SIZE/2.0f,
        .y = pos.y - EXTRA_BOX_SIZE/2.0f,
    };
    Color start_box_color = BOX_COLOR;

    Rectangle hs_box = start_box;
    hs_box.y += start_box.height + BOX_SPACING;
    Color hs_box_color = BOX_COLOR;

    Rectangle exit_box = hs_box;
    exit_box.y += hs_box.height + BOX_SPACING;
    Color exit_box_color = BOX_COLOR;

    Vector2 start_pos = Vector2Subtract(
                            Vector2Add(
                                (Vector2){.x = start_box.x, .y = start_box.y},
                                (Vector2){.x = start_box.width/2.0f, .y = start_box.height/2.0f}
                            ),
                            Vector2Scale(
                                start_size,
                                1/2.0f
                            )
                        );
    Vector2 hs_pos = Vector2Subtract(
                            Vector2Add(
                                (Vector2){.x = hs_box.x, .y = hs_box.y},
                                (Vector2){.x = hs_box.width/2.0f, .y = hs_box.height/2.0f}
                            ),
                            Vector2Scale(
                                hs_size,
                                1/2.0f
                            )
                        );
    Vector2 exit_pos = Vector2Subtract(
                            Vector2Add(
                                (Vector2){.x = exit_box.x, .y = exit_box.y},
                                (Vector2){.x = exit_box.width/2.0f, .y = exit_box.height/2.0f}
                            ),
                            Vector2Scale(
                                exit_size,
                                1/2.0f
                            )
                        );

    Vector2 cursor = GetMousePosition();
    if (CheckCollisionPointRec(cursor, start_box)) {
        start_box_color = BOX_COLOR_HOVERED;
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            HideCursor();
            return GAME_PLAYING;
        }
    } else if (CheckCollisionPointRec(cursor, hs_box)) {
        hs_box_color = BOX_COLOR_HOVERED;
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            return GAME_HIGHSCORE;
        }
    } else if (CheckCollisionPointRec(cursor, exit_box)) {
        exit_box_color = BOX_COLOR_HOVERED;
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            return GAME_EXIT;
        }
    }

    BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawRectangleRec(start_box, start_box_color);
        DrawRectangleRec(hs_box, hs_box_color);
        DrawRectangleRec(exit_box, exit_box_color);

        DrawTextEx(GetFontDefault(), start_text, start_pos, 40, 4, BLACK);
        DrawTextEx(GetFontDefault(), hs_text, hs_pos, 40, 4, BLACK);
        DrawTextEx(GetFontDefault(), exit_text, exit_pos, 40, 4, BLACK);
    EndDrawing();

    return GAME_START_MENU;
}

GameState hs_leaderboard(int screen_width, int screen_height) {
    if (display_leaderboard(screen_width, screen_height)) {
        return GAME_HIGHSCORE;
    }
    return GAME_START_MENU;
}

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

static inline unsigned int get_enemy_score() {
    return (rand() % ENEMY_MAX_SCORE) + 1;
}

static Color get_enemy_color(unsigned int score) {
    switch (score) {
    case 1:
        return GREEN;
    case 2:
        return YELLOW;
    case 3:
        return RED;
    default:
        assert("Unreachable");
        return RAYWHITE;
    }
}

static void reset_game_state(Entity *player, Entity *enemy, Coin *coin) {
    player->position = (Vector2){.x = SCREEN_WIDTH/2.0f, .y = 2.0f*SCREEN_HEIGHT/3.0f};
    player->velocity = Vector2Zero();
    player->score = 0;
    player->lives = 3;

    enemy->position = (Vector2){.x = SCREEN_WIDTH/2.0f, .y = SCREEN_HEIGHT/3.0f};
    enemy->velocity = (Vector2){.x = rand_float()*PLAYER_MAX_V/2.0f, .y = rand_float()*PLAYER_MAX_V/2.0f};
    enemy->lives = 5;
    enemy->score = get_enemy_score();
    enemy->color = get_enemy_color(enemy->score);

    coin->exists = false;
}

static GameState gameplay_loop(Entity *player, Entity *enemy, Coin *coin) {
    if (IsKeyPressed(KEY_R)) {
        ShowCursor();
        add_potential_highscore(player->score, SCREEN_WIDTH, SCREEN_HEIGHT);
        return GAME_START_MENU;
    }

    const float dt = GetFrameTime();
    bool collided = entity_elastic_collision(player, enemy);

    if (collided) {
        player->lives--;
        if (player->lives == 0) {
            ShowCursor();
            add_potential_highscore(player->score, SCREEN_WIDTH, SCREEN_HEIGHT);
            return GAME_START_MENU;
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
        if (player->score < 9999) {
            player->score++;
        }

        enemy->lives--;
        if (enemy->lives == 0) {
            unsigned int score = player->score + enemy->score;
            unsigned int lives = player->lives;
            reset_game_state(player, enemy, coin);
            player->score = score;
            player->lives = lives;
        }
    }

    update_text(player);
    BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawRectangleRec(player->sprite, player->color);
        DrawRectangleRec(enemy->sprite, enemy->color);

        if (coin->exists) {
            DrawRectangleRec(coin->sprite, coin->color);
        }

        DrawText(lives_text, 10, 10, 20, BLACK);
        DrawText(score_text, 10, 30, 20, BLACK);
    EndDrawing();

    return GAME_PLAYING;
}

int main(void) {
    srand(time(NULL));

    Entity player = {
        .position = {.x = SCREEN_WIDTH/2.0f, .y = 2.0f*SCREEN_HEIGHT/3.0f},
        .velocity = {0},
        .max_velocity = PLAYER_MAX_V,
        .score = 0,
        .lives = 3,
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
        .score = get_enemy_score(),
        .lives = 5,
        .sprite = {
            .width = ENTITY_WIDTH,
            .height = ENTITY_HEIGHT,
        },
    };
    enemy.color = get_enemy_color(enemy.score);

    Coin coin = {
        .sprite = {
            .width = 10,
            .height = 20,
        },
        .color = GOLD,
        .exists = false,
    };

    load_highscores_from_disk();
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Sliding block");
    SetTargetFPS(FPS);

    GameState state = GAME_START_MENU;
    bool exit_game = false;
    while (!WindowShouldClose() && !exit_game) {
        switch (state) {
            case GAME_START_MENU: {
                state = start_menu();
                if (state != GAME_START_MENU) {
                    reset_game_state(&player, &enemy, &coin);
                }
            } break;

            case GAME_PLAYING: {
                state = gameplay_loop(&player, &enemy, &coin);
            } break;

            case GAME_HIGHSCORE: {
                state = hs_leaderboard(SCREEN_WIDTH, SCREEN_HEIGHT);
            } break;

            case GAME_EXIT: {
                exit_game = true;
            } break;
        }
    }

    CloseWindow();
    write_highscores_to_disk();
    return 0;
}
