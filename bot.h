#ifndef _BOT_H_
#define _BOT_H_

#include "board.h"

#define MAX_PATH_LEN 100

typedef struct {
    int len;
    direction directions[MAX_PATH_LEN];
} path;

typedef struct {
    int line;
    int column;
} position;

typedef struct {
    position piece;
    path path;
} move;

typedef enum {EASY, MEDIUM, HARD} level;

move bot_move(board game, player bot);
void random_piece_placement(board game, player bot);
void set_difficulty(level choice);

#endif