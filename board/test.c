#include <stdio.h>
#include "board.h"

void affichage(board game) {
    for (int l = DIMENSION-1; l >= 0; l--) {
        for (int c = 0; c < DIMENSION; c++) {
            if (l==picked_piece_line(game) && c==picked_piece_column(game)) {
                printf("%d*", picked_piece_size(game));
            } else {
                printf("%d ", get_piece_size(game, l, c));
            }
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

    printf("%d\n", nb_pieces_available(game, ONE, NORTH_P));
    printf("%d\n", nb_pieces_available(game, TWO, NORTH_P));
    printf("%d\n", nb_pieces_available(game, THREE, NORTH_P));
    printf("%d\n", nb_pieces_available(game, ONE, SOUTH_P));
    printf("%d\n", nb_pieces_available(game, TWO, SOUTH_P));
    printf("%d\n", nb_pieces_available(game, THREE, SOUTH_P));

    printf("%d\n", nb_pieces_available(game, -1, NORTH_P));
    printf("%d\n", nb_pieces_available(game, 0, NORTH_P));
    printf("%d\n", nb_pieces_available(game, 4, NORTH_P));
    printf("%d\n", nb_pieces_available(game, ONE, 61));
    printf("%d\n", nb_pieces_available(game, 4, 0));
    printf("%d\n", nb_pieces_available(game, 4, 3));


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
    printf("%d\n", is_move_possible(game, NORTH)); 
    printf("%d\n", is_move_possible(game, SOUTH));
    printf("%d\n", is_move_possible(game, EAST));
    printf("%d\n", is_move_possible(game, WEST));
    printf("%d\n", is_move_possible(game, GOAL));
    printf("%d\n", picked_piece_size(game));
    printf("%d\n", picked_piece_owner(game));
    printf("%d\n", picked_piece_line(game));
    printf("%d\n", picked_piece_column(game));
    printf("%d\n", nb_pieces_available(game, ONE, NORTH_P));
    printf("%d\n", nb_pieces_available(game, TWO, NORTH_P));
    printf("%d\n", nb_pieces_available(game, THREE, NORTH_P));
    printf("%d\n", nb_pieces_available(game, ONE, SOUTH_P));
    printf("%d\n", nb_pieces_available(game, TWO, SOUTH_P));
    printf("%d\n", nb_pieces_available(game, THREE, SOUTH_P));
    
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

    printf("%d\n", movement_left(game)); // 30 initial variables
    printf("%d\n", picked_piece_size(game));
    printf("%d\n", movement_left(game));
    printf("%d\n", picked_piece_owner(game));
    printf("%d\n", picked_piece_line(game));
    printf("%d\n", movement_left(game));
    printf("%d\n", picked_piece_column(game));
    printf("%d\n", is_move_possible(game, NORTH)); 
    printf("%d\n", is_move_possible(game, SOUTH));
    printf("%d\n", is_move_possible(game, EAST));
    printf("%d\n", movement_left(game));
    printf("%d\n", is_move_possible(game, WEST));

    printf("%d\n", pick_piece(game, SOUTH_P, 1, 0)); // 35 test pick_piece
    printf("%d\n", pick_piece(game, SOUTH_P, 0, -1)); // wrong (line, column)
    printf("%d\n", pick_piece(game, SOUTH_P, -1, 0));
    printf("%d\n", is_move_possible(game, NORTH)); 
    printf("%d\n", movement_left(game));
    printf("%d\n", is_move_possible(game, SOUTH));
    printf("%d\n", is_move_possible(game, EAST));
    printf("%d\n", is_move_possible(game, WEST));
    printf("%d\n", movement_left(game));
    printf("%d\n", pick_piece(game, SOUTH_P, 42, 0));
    printf("%d\n", pick_piece(game, SOUTH_P, 0, 42));

    printf("%d\n", pick_piece(game, NORTH_P, 0, 0)); // 40 wrong player
    printf("%d\n", pick_piece(game, 0, 0, 0));
    printf("%d\n", movement_left(game));
    printf("%d\n", pick_piece(game, 3, 0, 0));
    printf("%d\n", pick_piece(game, -1, 0, 0));
    printf("%d\n", movement_left(game));
    printf("%d\n", pick_piece(game, 42, 0, 0));

    printf("%d\n", pick_piece(game, SOUTH_P, 0, 0)); // 45 normal

    affichage(game); // 46-51

    printf("%d\n", movement_left(game)); // 52 variables
    printf("%d\n", picked_piece_size(game));
    printf("%d\n", picked_piece_owner(game));
    printf("%d\n", picked_piece_line(game));
    printf("%d\n", is_move_possible(game, NORTH)); 
    printf("%d\n", is_move_possible(game, SOUTH));
    printf("%d\n", is_move_possible(game, EAST));
    printf("%d\n", is_move_possible(game, WEST));
    printf("%d\n", is_move_possible(game, GOAL));
    printf("%d\n", picked_piece_size(game));
    printf("%d\n", picked_piece_owner(game));
    printf("%d\n", picked_piece_line(game));
    printf("%d\n", picked_piece_column(game));
    printf("%d\n", nb_pieces_available(game, ONE, NORTH_P));
    printf("%d\n", nb_pieces_available(game, TWO, NORTH_P));
    printf("%d\n", nb_pieces_available(game, THREE, NORTH_P));
    printf("%d\n", nb_pieces_available(game, ONE, SOUTH_P));
    printf("%d\n", nb_pieces_available(game, TWO, SOUTH_P));
    printf("%d\n", nb_pieces_available(game, THREE, SOUTH_P));
    
    printf("%d\n", picked_piece_column(game));
    printf("%d\n", movement_left(game));

    /*
    1 1 2 2 3 3 
    0 0 0 0 0 0 
    0 0 0 0 0 0 
    0 0 0 0 0 0 
    0 0 0 0 0 0 
   (1) 3 2 3 1 2
    */

    printf("%d\n", is_move_possible(game, NORTH)); 
    printf("%d\n", movement_left(game));
    printf("%d\n", is_move_possible(game, SOUTH));
    printf("%d\n", is_move_possible(game, EAST));
    printf("%d\n", is_move_possible(game, WEST));

    printf("%d\n", pick_piece(game, SOUTH_P, 0, 1)); // 62
    printf("%d\n", is_move_possible(game, NORTH));
    printf("%d\n", is_move_possible(game, SOUTH));
    printf("%d\n", is_move_possible(game, EAST));
    printf("%d\n", is_move_possible(game, WEST));

    printf("%d\n", pick_piece(game, NORTH_P, 5, 0)); // 67
    printf("%d\n", is_move_possible(game, NORTH));
    printf("%d\n", movement_left(game));
    printf("%d\n", is_move_possible(game, SOUTH));
    printf("%d\n", is_move_possible(game, EAST));
    printf("%d\n", is_move_possible(game, WEST));

    printf("%d\n", pick_piece(game, NORTH_P, 5, 1)); // 72
    printf("%d\n", is_move_possible(game, NORTH));
    printf("%d\n", is_move_possible(game, SOUTH));
    printf("%d\n", is_move_possible(game, EAST));
    printf("%d\n", is_move_possible(game, WEST));

    affichage(game); // 77-82

    /*
    0(1)2 2 3 3 
    0 0 0 0 0 0 
    0 0 0 0 0 0 
    0 0 0 0 0 0 
    0 0 0 0 0 0 
    0 0 2 3 1 2
    */

    printf("%d\n", movement_left(game));// 83-93
    printf("%d\n", move_piece(game, NORTH)); // out of the board
    printf("%d\n", movement_left(game));
    printf("%d\n", move_piece(game, EAST)); // bounce
    printf("%d\n", movement_left(game));

    printf("%d\n", swap_piece(game, -1, 0));
    printf("%d\n", swap_piece(game, 0, -1));
    printf("%d\n", swap_piece(game, 6, 0));
    printf("%d\n", swap_piece(game, 0, 6));
    printf("%d\n", swap_piece(game, 5, 2));
    printf("%d\n", swap_piece(game, 0, 5));
    printf("%d\n", swap_piece(game, 0, 0));

    printf("%d\n", move_piece(game, EAST)); // blocking piece
    printf("%d\n", movement_left(game));
    printf("%d\n", move_piece(game, SOUTH)); // normal
    printf("%d\n", movement_left(game));
    printf("%d\n", move_piece(game, SOUTH)); // normal -> finished
    printf("%d\n", movement_left(game));

    affichage(game); // 94 - 99

    /*
    0 0 2 2 3 3 
    0 0 0 0 0 0 
    0 0 1 0 0 0 
    0 0 0 0 0 0 
    0 0 0 0 0 0 
    0 0 2 3 1 2
    */

    printf("%d\n", pick_piece(game, NORTH_P, 5, 5)); // 100 normal path
    printf("%d\n", move_piece(game, SOUTH));
    printf("%d\n", move_piece(game, WEST));
    printf("%d\n", is_move_possible(game, NORTH)); 
    printf("%d\n", is_move_possible(game, SOUTH));
    printf("%d\n", movement_left(game));
    printf("%d\n", is_move_possible(game, EAST));
    printf("%d\n", is_move_possible(game, WEST));
    printf("%d\n", move_piece(game, NORTH));
    printf("%d\n", move_piece(game, SOUTH));
    printf("%d\n", is_move_possible(game, NORTH)); 
    printf("%d\n", movement_left(game));
    printf("%d\n", is_move_possible(game, SOUTH));
    printf("%d\n", is_move_possible(game, EAST));
    printf("%d\n", is_move_possible(game, WEST));
    printf("%d\n", move_piece(game, SOUTH));
    
    /*
    0 0 2 2 3 0 
    0 0 0 0 0 0 
    0 0 1 0(3)0 
    0 0 0 0 0 0 
    0 0 0 0 0 0 
    0 0 2 3 1 2
    */

    printf("%d\n", movement_left(game)); // 106
    affichage(game);

    printf("%d\n", cancel_step(game));
    printf("%d\n", is_move_possible(game, NORTH)); 
    printf("%d\n", is_move_possible(game, SOUTH));
    printf("%d\n", is_move_possible(game, EAST));
    printf("%d\n", is_move_possible(game, WEST));
    printf("%d\n", is_move_possible(game, GOAL));
    printf("%d\n", picked_piece_size(game));
    printf("%d\n", picked_piece_owner(game));
    printf("%d\n", picked_piece_line(game));
    printf("%d\n", picked_piece_column(game));
    printf("%d\n", nb_pieces_available(game, ONE, NORTH_P));
    printf("%d\n", nb_pieces_available(game, TWO, NORTH_P));
    printf("%d\n", nb_pieces_available(game, THREE, NORTH_P));
    printf("%d\n", nb_pieces_available(game, ONE, SOUTH_P));
    printf("%d\n", nb_pieces_available(game, TWO, SOUTH_P));
    printf("%d\n", nb_pieces_available(game, THREE, SOUTH_P));
    
    printf("%d\n", movement_left(game));
    affichage(game);

    printf("%d\n", cancel_step(game));
    printf("%d\n", movement_left(game));
    affichage(game);

    printf("%d\n", cancel_step(game));
    printf("%d\n", movement_left(game));
    printf("%d\n", is_move_possible(game, NORTH)); 
    printf("%d\n", is_move_possible(game, SOUTH));
    printf("%d\n", is_move_possible(game, EAST));
    printf("%d\n", is_move_possible(game, WEST));
    printf("%d\n", is_move_possible(game, GOAL));
    printf("%d\n", picked_piece_size(game));
    printf("%d\n", picked_piece_owner(game));
    printf("%d\n", picked_piece_line(game));
    printf("%d\n", picked_piece_column(game));
    printf("%d\n", nb_pieces_available(game, ONE, NORTH_P));
    printf("%d\n", nb_pieces_available(game, TWO, NORTH_P));
    printf("%d\n", nb_pieces_available(game, THREE, NORTH_P));
    printf("%d\n", nb_pieces_available(game, ONE, SOUTH_P));
    printf("%d\n", nb_pieces_available(game, TWO, SOUTH_P));
    printf("%d\n", nb_pieces_available(game, THREE, SOUTH_P));
    
    affichage(game);

    printf("%d\n", cancel_step(game));
    printf("%d\n", movement_left(game));
    affichage(game);
    printf("%d\n", is_move_possible(game, NORTH)); 
    printf("%d\n", is_move_possible(game, SOUTH));
    printf("%d\n", is_move_possible(game, EAST));
    printf("%d\n", is_move_possible(game, WEST));
    printf("%d\n", is_move_possible(game, GOAL));
    printf("%d\n", picked_piece_size(game));
    printf("%d\n", picked_piece_owner(game));
    printf("%d\n", picked_piece_line(game));
    printf("%d\n", picked_piece_column(game));
    printf("%d\n", nb_pieces_available(game, ONE, NORTH_P));
    printf("%d\n", nb_pieces_available(game, TWO, NORTH_P));
    printf("%d\n", nb_pieces_available(game, THREE, NORTH_P));
    printf("%d\n", nb_pieces_available(game, ONE, SOUTH_P));
    printf("%d\n", nb_pieces_available(game, TWO, SOUTH_P));
    printf("%d\n", nb_pieces_available(game, THREE, SOUTH_P));
    
    printf("%d\n", cancel_step(game));
    printf("%d\n", movement_left(game));
    affichage(game);

    printf("%d\n", cancel_step(game));
    printf("%d\n", movement_left(game));
    affichage(game);
    
    printf("%d\n", cancel_step(game));
    printf("%d\n", movement_left(game));
    affichage(game);

    /*
    0 0 2 2 3 3 
    0 0 0 0 0 0 
    0 0 1 0 0 0 
    0 0 0 0 0 0 
    0 0 0 0 0 0 
    0 0 2 3 1 2
    */

    printf("%d\n", pick_piece(game, SOUTH_P, 0, 3)); // 169 
    affichage(game);
    printf("%d\n", move_piece(game, NORTH));
    printf("%d\n", move_piece(game, SOUTH));
    printf("%d\n", move_piece(game, NORTH)); // forbidden move

    affichage(game);
    printf("%d\n", cancel_movement(game));
    affichage(game);

    printf("%d\n", place_piece(game, ONE, NORTH, 0));
    printf("%d\n", pick_piece(game, SOUTH_P, 0, 2)); 
    printf("%d\n", pick_piece(game, SOUTH_P, 0, 3)); 
    printf("%d\n", is_move_possible(game, NORTH)); 
    printf("%d\n", is_move_possible(game, SOUTH));
    printf("%d\n", is_move_possible(game, EAST));
    printf("%d\n", is_move_possible(game, WEST));
    printf("%d\n", is_move_possible(game, GOAL));
    printf("%d\n", picked_piece_size(game));
    printf("%d\n", picked_piece_owner(game));
    printf("%d\n", picked_piece_line(game));
    printf("%d\n", picked_piece_column(game));
    printf("%d\n", nb_pieces_available(game, ONE, NORTH_P));
    printf("%d\n", nb_pieces_available(game, TWO, NORTH_P));
    printf("%d\n", nb_pieces_available(game, THREE, NORTH_P));
    printf("%d\n", nb_pieces_available(game, ONE, SOUTH_P));
    printf("%d\n", nb_pieces_available(game, TWO, SOUTH_P));
    printf("%d\n", nb_pieces_available(game, THREE, SOUTH_P));
    
    printf("%d\n", pick_piece(game, SOUTH_P, 0, 4)); 
    printf("%d\n", pick_piece(game, SOUTH_P, 0, 5)); 
    affichage(game);
    printf("%d\n", cancel_movement(game));
    affichage(game);
    printf("%d\n", is_move_possible(game, NORTH)); 
    printf("%d\n", is_move_possible(game, SOUTH));
    printf("%d\n", is_move_possible(game, EAST));
    printf("%d\n", is_move_possible(game, WEST));
    printf("%d\n", is_move_possible(game, GOAL));
    printf("%d\n", picked_piece_size(game));
    printf("%d\n", picked_piece_owner(game));
    printf("%d\n", picked_piece_line(game));
    printf("%d\n", picked_piece_column(game));
    printf("%d\n", nb_pieces_available(game, ONE, NORTH_P));
    printf("%d\n", nb_pieces_available(game, TWO, NORTH_P));
    printf("%d\n", nb_pieces_available(game, THREE, NORTH_P));
    printf("%d\n", nb_pieces_available(game, ONE, SOUTH_P));
    printf("%d\n", nb_pieces_available(game, TWO, SOUTH_P));
    printf("%d\n", nb_pieces_available(game, THREE, SOUTH_P));
    

    printf("%d\n", pick_piece(game, SOUTH_P, 5, 0)); 
    printf("%d\n", is_move_possible(game, NORTH)); 
    printf("%d\n", is_move_possible(game, SOUTH));
    printf("%d\n", is_move_possible(game, EAST));
    printf("%d\n", is_move_possible(game, WEST));
    printf("%d\n", is_move_possible(game, GOAL));
    printf("%d\n", picked_piece_size(game));
    printf("%d\n", picked_piece_owner(game));
    printf("%d\n", picked_piece_line(game));
    printf("%d\n", picked_piece_column(game));
    printf("%d\n", nb_pieces_available(game, ONE, NORTH_P));
    printf("%d\n", nb_pieces_available(game, TWO, NORTH_P));
    printf("%d\n", nb_pieces_available(game, THREE, NORTH_P));
    printf("%d\n", nb_pieces_available(game, ONE, SOUTH_P));
    printf("%d\n", nb_pieces_available(game, TWO, SOUTH_P));
    printf("%d\n", nb_pieces_available(game, THREE, SOUTH_P));
    
    printf("%d\n", move_piece(game, GOAL));
    affichage(game);
















    return 0;
}