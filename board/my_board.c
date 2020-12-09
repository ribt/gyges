#include <stdlib.h>
#include <stdio.h> /////////////// TO REMOVE
#include "board.h"


struct board_s {
    size map[DIMENSION][DIMENSION];
    size picked_piece_size;
    player current_player;
    int picked_piece_line;
    int picked_piece_column;
    int movement_left;
    player winner;
};

board new_game() {
    //printf("You're using my own board.c!\n"); /////////////////////// TO REMOVE
    board new_board = malloc(sizeof(struct board_s));

    for (int line = 0; line < DIMENSION; line++) {
        for (int column = 0; column < DIMENSION; column++) {
            new_board->map[column][line] = NONE;
        }
    }

    new_board->picked_piece_size = NONE;
    new_board->current_player = NO_PLAYER;
    new_board->picked_piece_size = -1;
    new_board->picked_piece_size = -1;
    new_board->movement_left = -1;
    new_board->winner = NO_PLAYER;

    return new_board;
}

void destroy_game(board game) {
    free(game);
}

size get_piece_size(board game, int line, int column) {
    return game->map[line][column];
}

player get_winner(board game){
    return game->winner;
}

int southmost_occupied_line(board game) {
    for(int line = 0; line < DIMENSION; line++) {
        for (int column = 0; column < DIMENSION; column++) {
            if (get_piece_size(game, line, column) != NONE) {
                return line;
            }
        }
    }

    return -1;
}

int northmost_occupied_line(board game) {
    for(int line = DIMENSION-1; line >= 0; line++) {
        for (int column = 0; column < DIMENSION; column++) {
            if (get_piece_size(game, line, column) != NONE) {
                return line;
            }
        }
    }

    return -1;
}


player picked_piece_owner(board game) {
    return game->current_player;
}

size picked_piece_size(board game) {
    return game->picked_piece_size;
}

int picked_piece_line(board game) {
    return game->picked_piece_line;
}

int picked_piece_column(board game) {
    return game->picked_piece_column;
}

int movement_left(board game) {
    return game->movement_left;
}



int nb_pieces_available(board game, size piece, player player) {
    int line, count;

    if (piece != ONE && piece != TWO && piece != THREE) {
        return -1;
    }

    if (player == NORTH_P) {
        line = DIMENSION-1;
    } else if (player == SOUTH_P) {
        line = 0;
    } else {
        return -1;
    }

    count = NB_INITIAL_PIECES;

    for (int i = 0; i < DIMENSION; i++) {
        if (game->map[line][i] == piece) {
            count--;
        }
    }

    return count;
}

return_code place_piece(board game, size piece, player player, int column) {
    int line, count;

    if (piece != ONE && piece != TWO && piece != THREE) {
        return PARAM;
    }

    if (column < 0 || column >= DIMENSION) {
        return PARAM;
    }

    if (player == NORTH_P) {
        line = DIMENSION-1;
    } else if (player == SOUTH_P) {
        line = 0;
    } else {
        return PARAM;
    }

    if (game->map[line][column] != NONE) {
        return EMPTY;
    }

    count = 0;
    for (int i = 0; i < DIMENSION; i++) {
        if (game->map[line][i] == piece) {
            count++;
        }
    }
    if (count >= NB_INITIAL_PIECES) {
        return FORBIDDEN;
    }

    game->map[line][column] = piece;

    return OK;
}



// TODO:

return_code pick_piece(board game, player current_player, int line, int column);

bool is_move_possible(board game, direction direction);


return_code move_piece(board game, direction direction);

return_code swap_piece(board game, int target_line, int target_column);

return_code cancel_movement(board game);

return_code cancel_step(board game);