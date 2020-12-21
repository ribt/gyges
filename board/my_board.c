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


board new_game(){
    board new_board = malloc(sizeof(struct board_s));

    for (int line = 0; line < DIMENSION; line++) {
        for (int column = 0; column < DIMENSION; column++) {
            new_board->map[column][line] = NONE;
        }
    }

    new_board->picked_piece_size = NONE;
    new_board->current_player = NO_PLAYER;
    new_board->picked_piece_line = -1;
    new_board->picked_piece_column = -1;
    new_board->movement_left = -1;
    new_board->winner = NO_PLAYER;

    return new_board;
}

void destroy_game(board game) {
    free(game);
}

size get_piece_size(board game, int line, int column) {
    if (line < 0 || line >= DIMENSION || column < 0 || column >= DIMENSION) {
        return NONE;
    }
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

return_code pick_piece(board game, player current_player, int line, int column) {
    int right_line;

    if (current_player == NORTH_P) {
        right_line = northmost_occupied_line(game);
    } else if (current_player == SOUTH_P) {
        right_line = southmost_occupied_line(game);
    } else {
        return PARAM;
    }

    if (line >= DIMENSION || line < 0 || column >= DIMENSION || column < 0) {
        return PARAM;
    }

    if (game->map[line][column] == NONE) {
        return EMPTY;
    }

    if (line != right_line || get_winner(game) != NO_PLAYER) {
        return FORBIDDEN;
    }

    game->picked_piece_line = line;
    game->picked_piece_column = column;
    game->picked_piece_size = game->map[line][column];

    game->map[line][column] = 0;
    game->current_player = current_player;
    game->movement_left = game->picked_piece_size;

    return OK;
}

bool is_goal_reachable(board game) {
    int movement = game->movement_left;
    
    if (movement == 0) {
        movement = get_piece_size(game, game->picked_piece_line, game->picked_piece_column); 
    }

    if (game->current_player == NORTH_P && game->picked_piece_line == 0 &&  movement == 1) {
        return true;
    }

    if (game->current_player == SOUTH_P && game->picked_piece_line == DIMENSION-1 &&  movement == 1) {
        return true;
    }

    return false;
}

bool is_move_possible(board game, direction testing_direction) {
    int next_line = game->picked_piece_line;
    int next_column = game->picked_piece_column;

    if (game->picked_piece_size == NONE || game->movement_left < 1) {
        return false;
    }

    switch (testing_direction) {
        case NORTH: 
            next_line++;
            break;
        case SOUTH: 
            next_line--;
            break;
        case EAST: 
            next_column++;
            break;
        case WEST: 
            next_column--;
            break;

        case GOAL:
            return is_goal_reachable(game);
    }

    if (next_line < 0 || next_line >= DIMENSION || next_column < 0 || next_column >= DIMENSION) {
        return false;
    }
    if (game->map[next_line][next_column] != NONE) {
        if (game->movement_left == 1) {
            return true;
        } else {
            return false;
        }
    }

    return true;
}

void terminate_move(board game) {
    game->map[picked_piece_line(game)][picked_piece_column(game)] = picked_piece_size(game);
    game->movement_left = -1;
    game->current_player = NONE;
    game->picked_piece_line = -1;
    game->picked_piece_column = -1;
}

return_code move_piece(board game, direction direction) {
    int next_line = game->picked_piece_line;
    int next_column = game->picked_piece_column;
    int movement = game->movement_left;

    if (picked_piece_size(game) == NONE) {
        return EMPTY;
    }

    if (movement_left(game) == -1) {
        return PARAM;
    }

    if (picked_piece_owner(game) == NO_PLAYER) {
        return PARAM;
    }

    if (direction == GOAL && is_goal_reachable(game) == false) {
        return FORBIDDEN;
    }

    switch (direction) {
        case NORTH: 
            next_line++;
            break;
        case SOUTH: 
            next_line--;
            break;
        case EAST: 
            next_column++;
            break;
        case WEST: 
            next_column--;  
            break;
        case GOAL:
            break;
    }

    if (next_line < 0 || next_line >= DIMENSION || next_column < 0 || next_column >= DIMENSION) {
        return PARAM;
    }

    if (movement == 0){
        movement = get_piece_size(game, game->picked_piece_line, game->picked_piece_column);
    }

    if (game->map[next_line][next_column] != NONE) {
        if (movement != 1 ){  //Il y a quelques chose et il reste pas un seul déplacement
            return FORBIDDEN;
        } else {    //Il y a quelque chose et il reste un seul déplacement
            game->picked_piece_line = next_line;
            game->picked_piece_column = next_column; 
            movement --;
        }
    } else if (game->movement_left == 1) {  //Il y a rien et il ne reste qu'un seul déplacement
        game->picked_piece_line = next_line;
        game->picked_piece_column = next_column;
        terminate_move(game);
    }
    else {  //Il y a rien et il reste plusieurs déplacements
        game->picked_piece_line = next_line;
        game->picked_piece_column = next_column; 
        movement --;
    }

    if (game->movement_left != -1) {
        game->movement_left = movement;
    }
    return OK;

}

return_code swap_piece(board game, int target_line, int target_column) {
    if (game->movement_left != 0) {
        return EMPTY;
    }
    if (target_line < 0 || target_line >= DIMENSION || target_column < 0 || target_column >= DIMENSION) {
        return PARAM;
    }
    if (game->map[target_line][target_column] != NONE
            || (target_line == game->picked_piece_line && target_column == game->picked_piece_column)
            || game->current_player == NO_PLAYER) {
        return FORBIDDEN;
    }

    game->map[target_line][target_column] = get_piece_size(game, picked_piece_line(game), picked_piece_column(game));
    
    terminate_move(game);

    return OK;
}



// TODO:

return_code cancel_movement(board game);

return_code cancel_step(board game);
