#include <stdio.h>
#include "board.h"
#include "display.h"

/**
 * @file game.c
 * @brief a simple Gygès game
 * @author ribt
 */
int main(){
    board game = new_game();

    place_piece(game, THREE, SOUTH_P, 0);
    place_piece(game, ONE, SOUTH_P, 1);
    place_piece(game, TWO, SOUTH_P, 2);
    place_piece(game, THREE, SOUTH_P, 3);
    place_piece(game, ONE, SOUTH_P, 4);
    place_piece(game, TWO, SOUTH_P, 5);

    place_piece(game, THREE, NORTH_P, 0);
    place_piece(game, ONE, NORTH_P, 1);
    place_piece(game, TWO, NORTH_P, 2);
    place_piece(game, THREE, NORTH_P, 3);
    place_piece(game, ONE, NORTH_P, 4);
    place_piece(game, TWO, NORTH_P, 5);
    

    disp_board(game);
  
    return 0;
}


