#ifndef HIGHSCORE_H
#define HIGHSCORE_H

#include <raylib.h>

#include <ctype.h>
#include <stdio.h>
#include <string.h>

/**
 * Add a pontential highscore to a list of top 10 highscores. If the given score is zero,
 * the function does nothing.
 *
 * @param score The potential score to be added.
 * @param screen_width The wiidth of the screen for prompting purposes.
 * @param screen_height The wiidth of the screen for prompting purposes.
 */
void add_potential_highscore(unsigned int score, int screen_width, int screen_height);

/**
 * Display the highscore leaderboard.
 *
 * @param screen_width The width of the screen.
 * @param screen_height The height of the screen.
 * @return bool Whether "back" has been pressed or not.
 */
bool display_leaderboard(int screen_width, int screen_height);

/**
 * Load a list of top 10 highscores from a file named "save.csv" into memory.
 */
void load_highscores_from_disk(void);

/**
 * Save the current highscores to a file named "save.csv".
 */
void write_highscores_to_disk(void);

#endif // HIGHSCORE_H
