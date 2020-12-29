#include <stdio.h>
#include "board.h"
#include "bot.h"
#include "display.h"

void set_map(board game, int map[DIMENSION][DIMENSION]);

int main() {
    board game = new_game();
    int map[DIMENSION][DIMENSION] = {
        {3, 0, 2, 0, 0, 3},
        {0, 0, 0, 0, 0, 3},
        {0, 0, 1, 0, 2, 0},
        {0, 0, 2, 0, 1, 1},
        {0, 0, 0, 0, 0, 2},
        {1, 3, 0, 0, 0, 0}
    };

    set_map(game, map);

    clear_screen();

    bot_move(game, SOUTH_P);
    

}