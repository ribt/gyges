#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "board.h"
#include "bot.h"
#include "display.h"

int main() {
    srand(time(NULL));
    board game = new_game();
    player current_player;

    random_piece_placement(game, NORTH_P);
    random_piece_placement(game, SOUTH_P);

    current_player = NORTH_P;

    while (get_winner(game) == NO_PLAYER) {
        clear_screen();
        disp_board(game);
        usleep(750*1000);
        clear_screen();
        bot_move(game, current_player);
        current_player = next_player(current_player);
    }

    clear_screen();
    disp_board(game);

}
