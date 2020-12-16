#include <stdio.h>
#include "board.h"

void affichage(board game) {
    for (int l = DIMENSION-1; l >= 0; l--) {
        for (int c = 0; c < DIMENSION; c++) {
            printf("%d ", get_piece_size(game, l, c));
        }
        printf("\n");
    }
}

int main() {
    board game = new_game();

    printf("%d\n", place_piece(game, ONE, SOUTH_P, 0)); // 1 normal usage
    printf("%d\n", place_piece(game, THREE, SOUTH_P, 1));
    printf("%d\n", place_piece(game, TWO, SOUTH_P, 2));
    printf("%d\n", place_piece(game, THREE, SOUTH_P, 3));
    printf("%d\n", place_piece(game, ONE, SOUTH_P, 4));
    printf("%d\n", place_piece(game, TWO, SOUTH_P, 5));

    printf("%d\n", place_piece(game, ONE, NORTH_P, 0)); // 7 normal usage
    printf("%d\n", place_piece(game, ONE, NORTH_P, 1));
    printf("%d\n", place_piece(game, TWO, NORTH_P, 2));
    printf("%d\n", place_piece(game, TWO, NORTH_P, 3));
    printf("%d\n", place_piece(game, THREE, NORTH_P, 4));

    printf("%d\n", place_piece(game, THREE, NORTH_P, 0)); // 12 wrong col
    printf("%d\n", place_piece(game, THREE, NORTH_P, 6));
    printf("%d\n", place_piece(game, THREE, NORTH_P, -1));

    printf("%d\n", place_piece(game, ONE, NORTH_P, 5)); // 15 wrong size
    printf("%d\n", place_piece(game, TWO, NORTH_P, 5));
    printf("%d\n", place_piece(game, NONE, NORTH_P, 5));
    printf("%d\n", place_piece(game, THREE+1, NORTH_P, 5));

    printf("%d\n", place_piece(game, THREE, SOUTH_P, 5)); // 19 wrong player
    printf("%d\n", place_piece(game, THREE, -1, 5));
    printf("%d\n", place_piece(game, THREE, 0, 5));
    printf("%d\n", place_piece(game, THREE, 3, 5));

    printf("%d\n", place_piece(game, THREE, NORTH_P, 5)); // 23 normal

    affichage(game); // 24-29 display board

    /*
    1 1 2 2 3 3 
    0 0 0 0 0 0 
    0 0 0 0 0 0 
    0 0 0 0 0 0 
    0 0 0 0 0 0 
    1 3 2 3 1 2
    */

    printf("%d\n", movement_left(game)); // 30 initial movement_left

    printf("%d\n", pick_piece(game, SOUTH_P, 1, 0)); // 31 test pick_piece
    printf("%d\n", pick_piece(game, SOUTH_P, 0, -1)); // wrong (line, column)
    printf("%d\n", pick_piece(game, SOUTH_P, -1, 0));
    printf("%d\n", pick_piece(game, SOUTH_P, 42, 0));
    printf("%d\n", pick_piece(game, SOUTH_P, 0, 42));

    printf("%d\n", pick_piece(game, NORTH_P, 0, 0)); // 36 wrong player
    printf("%d\n", pick_piece(game, 0, 0, 0));
    printf("%d\n", pick_piece(game, 3, 0, 0));
    printf("%d\n", pick_piece(game, -1, 0, 0));
    printf("%d\n", pick_piece(game, 42, 0, 0));

    printf("%d\n", pick_piece(game, SOUTH_P, 0, 0)); // 41 normal

    affichage(game); // 42-47






    return 0;
}