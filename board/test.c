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

    printf("%d\n", place_piece(game, ONE, SOUTH_P, 0));
    printf("%d\n", place_piece(game, THREE, SOUTH_P, 1));
    printf("%d\n", place_piece(game, TWO, SOUTH_P, 2));
    printf("%d\n", place_piece(game, THREE, SOUTH_P, 3));
    printf("%d\n", place_piece(game, ONE, SOUTH_P, 4));
    printf("%d\n", place_piece(game, TWO, SOUTH_P, 5));

    printf("%d\n", place_piece(game, ONE, NORTH_P, 0));
    printf("%d\n", place_piece(game, ONE, NORTH_P, 1));
    printf("%d\n", place_piece(game, TWO, NORTH_P, 2));
    printf("%d\n", place_piece(game, TWO, NORTH_P, 3));
    printf("%d\n", place_piece(game, THREE, NORTH_P, 4));

    printf("%d\n", place_piece(game, THREE, NORTH_P, 0));
    printf("%d\n", place_piece(game, THREE, NORTH_P, 6));
    printf("%d\n", place_piece(game, THREE, NORTH_P, -1));

    printf("%d\n", place_piece(game, ONE, NORTH_P, 5));
    printf("%d\n", place_piece(game, TWO, NORTH_P, 5));
    printf("%d\n", place_piece(game, NONE, NORTH_P, 5));
    printf("%d\n", place_piece(game, THREE+1, NORTH_P, 5));

    printf("%d\n", place_piece(game, THREE, SOUTH_P, 5));
    printf("%d\n", place_piece(game, THREE, -1, 5));
    printf("%d\n", place_piece(game, THREE, 0, 5));
    printf("%d\n", place_piece(game, THREE, 3, 5));

    printf("%d\n", place_piece(game, THREE, NORTH_P, 5));

    affichage(game);

    printf("%d\n", movement_left(game));

    return 0;
}