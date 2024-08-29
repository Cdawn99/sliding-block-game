#include "highscore.h"
#include "save_filepath.h"

#define MAX_NAME_LEN 10
#define MAX_HS_COUNT 10

#define FONT_SIZE 30
#define SCORE_FONT_SIZE 37

#define BOX_COLOR SKYBLUE
#define BOX_COLOR_HOVERED BLUE

typedef struct {
    char name[MAX_NAME_LEN + 1];
    unsigned int score;
} Highscore;

Highscore hs[MAX_HS_COUNT];
unsigned int hs_count = 0;

static void shift_hs_elems(unsigned int from_idx) {
    unsigned int i = hs_count;
    if (i == MAX_HS_COUNT) {
        i--;
    }
    for (; i > from_idx; i--) {
        hs[i] = hs[i - 1];
    }
}

static void get_score_name(unsigned int at_idx, int screen_width, int screen_height) {
    const char prompt1[] = "High score!";
    const char prompt2[] = "Enter a name and press enter:";

    char teoretical_longest[] = "DDDDDDDDDD";
    Vector2 biggest_size = MeasureTextEx(GetFontDefault(), teoretical_longest, FONT_SIZE, FONT_SIZE/10.0f);

    Rectangle textbox = {
        .width = biggest_size.x + 10,
        .height = biggest_size.y + 5,
    };
    textbox.x = (screen_width - textbox.width)/2.0f;
    textbox.y = (screen_height - textbox.height)/2.0f;

    char name[11] = {0};
    unsigned int name_len = 0;
    while (true) {
        if (IsKeyPressed(KEY_ENTER)) {
            break;
        }

        if (IsKeyPressed(KEY_BACKSPACE) && name_len > 0) {
            name[--name_len] = '\0';
        }

        int key = GetCharPressed();
        if (key > 0 && name_len < MAX_NAME_LEN && isalpha(key)) {
            name[name_len++] = (char)key;
        }

        BeginDrawing();
            // ClearBackground(RAYWHITE);
            DrawRectangleRec(textbox, LIGHTGRAY);
            DrawText(prompt1, textbox.x, textbox.y - 90, FONT_SIZE, BLACK);
            DrawText(prompt2, textbox.x - textbox.width/2, textbox.y - 45, FONT_SIZE, BLACK);
            DrawText(name, textbox.x + 5, textbox.y + 5, FONT_SIZE, BLACK);
        EndDrawing();
    }

    strcpy(hs[at_idx].name, name);
}

void add_potential_highscore(unsigned int score, int screen_width, int screen_height) {
    if (score == 0) {
        return;
    }

    unsigned int hs_idx = 0;
    for (; hs_idx < hs_count; hs_idx++) {
        if (hs[hs_idx].score < score) {
            break;
        }
    }

    if (hs_idx >= MAX_HS_COUNT) {
        return;
    }

    if (hs_idx < hs_count) {
        shift_hs_elems(hs_idx);
    }

    hs[hs_idx].score = score;
    get_score_name(hs_idx, screen_width, screen_height);
    if (hs_count < MAX_HS_COUNT) {
        hs_count++;
    }
}

bool display_leaderboard(int screen_width, int screen_height) {
    // Place. Name, Score
    char teoretical_longest[] = "DD. DDDDDDDDDD, DDDD";
    Vector2 biggest_size = MeasureTextEx(GetFontDefault(), teoretical_longest, SCORE_FONT_SIZE, SCORE_FONT_SIZE/10.0f);

    char return_text[] = "Return";
    float return_text_w = MeasureText(return_text, SCORE_FONT_SIZE);

    char text[sizeof(teoretical_longest) - 1] = "High scores:";

    int x = screen_width/2 - (int)biggest_size.x/2;
    int y = 10;

    BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText(text, x, y, SCORE_FONT_SIZE, BLACK);
        for (int i = 0; i < hs_count; i++) {
            sprintf(text, "%2d. %s, %d", i + 1, hs[i].name, hs[i].score);
            y += biggest_size.y + 5;
            DrawText(text, x, y, SCORE_FONT_SIZE, DARKBLUE);
        }
        y += biggest_size.y + 5;

        Rectangle return_text_rec = {
            .x = x,
            .y = y,
            .width = biggest_size.x,
            .height = biggest_size.y + 5,
        };
        Color return_text_c = BOX_COLOR;
        Vector2 cursor = GetMousePosition();
        if (CheckCollisionPointRec(cursor, return_text_rec)) {
            return_text_c = BOX_COLOR_HOVERED;
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                return false;
            }
        }

        DrawRectangleRec(return_text_rec, return_text_c);
        DrawText(return_text, x + biggest_size.x/2 - return_text_w/2, y + 2.5, SCORE_FONT_SIZE, BLACK);
    EndDrawing();

    return true;
}

void load_highscores_from_disk(void) {
    char *save_text = LoadFileText(save_path);
    if (!save_text) {
        TraceLog(LOG_WARNING, "Failed to open %s", save_path);
        return;
    }

    char *it = save_text;
    while (*it != '\n') {
        it++;
    }
    it++;

    while (*it != '\0') {
        char *temp = it;

        while (*temp != ',') {
            temp++;
        }
        strncpy(hs[hs_count].name, it, temp - it);

        it = temp + 2;
        temp = it;
        while (*temp != '\n') {
            temp++;
        }
        unsigned int score = 0;
        for (; it != temp; it++) {
            score *= 10;
            score += *it - '0';
        }
        hs[hs_count].score = score;

        hs_count++;

        it++;
    }

    UnloadFileText(save_text);
}

void write_highscores_to_disk(void) {
    char save_text[256] = "name, score\n";

    char temp[23] = {0};
    for (unsigned int i = 0; i < hs_count; i++) {
        sprintf(temp, "%s, %u\n", hs[i].name, hs[i].score);
        strcat(save_text, temp);
    }

    bool succ = SaveFileText(save_path, save_text);
    if (!succ) {
        TraceLog(LOG_ERROR, "ERROR: Failed to save to %s", save_path);
    }
}
