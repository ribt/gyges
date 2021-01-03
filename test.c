#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "board.h"
#include "bot.h"
#include "display.h"

void set_map(board game, int map[DIMENSION][DIMENSION]);

int main() {
    srand(time(NULL));
    board game = new_game();
    // int map[DIMENSION][DIMENSION] = {
    //     {3, 0, 2, 0, 0, 3},
    //     {0, 0, 0, 0, 0, 3},
    //     {0, 0, 1, 0, 2, 0},
    //     {0, 0, 2, 0, 1, 1},
    //     {0, 0, 0, 0, 0, 2},
    //     {1, 3, 0, 0, 0, 0}
    // };

    // set_map(game, map);

    // clear_screen();

    // bot_move(game, SOUTH_P);

    // int map[DIMENSION][DIMENSION] = {
    //     {0, 0, 0, 0, 0, 0},
    //     {0, 0, 0, 0, 0, 0},
    //     {0, 0, 0, 0, 0, 0},
    //     {0, 0, 3, 2, 0, 0},
    //     {1, 0, 0, 2, 1, 3},
    //     {0, 0, 0, 0, 0, 0}
    // };


    random_piece_placement(game, NORTH_P);
    random_piece_placement(game, SOUTH_P);

    while (get_winner(game) == NO_PLAYER) {
        clear_screen();
        //printf("\nNorth Player :\n");
        bot_move(game, NORTH_P);
        if (get_winner(game)) {break;}
        clear_screen();
        //printf("\nSouth Player :\n");
        bot_move(game, SOUTH_P);
    }

    clear_screen();
    disp_board(game);

}