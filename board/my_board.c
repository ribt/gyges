#include <stdlib.h>
#include "board.h"

#define MAX_HISTORY_SIZE 100

struct board_s {
    size map[DIMENSION][DIMENSION];
    size picked_piece_size;
    player current_player;
    int picked_piece_line;
    int picked_piece_column;
    int movement_left;
    int positions_history[MAX_HISTORY_SIZE][2];
    int history_len;
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
    new_board->history_len = 0;

    return new_board;
}

void destroy_game(board game) {
    free(game);
}

bool are_coordinates_valid(int line, int column) {
    return line >= 0 && line < DIMENSION && column >= 0 && column < DIMENSION;
}

size get_piece_size(board game, int line, int column) {
    if (!are_coordinates_valid(line, column)) {
        return NONE;
    }
    return game->map[line][column];
}

player get_winner(board game){
    return game->winner;
}

player next_player(player current_player) {
    if (current_player == NORTH_P) {
        return SOUTH_P;
    } else if (current_player == SOUTH_P) {
        return NORTH_P;
    } else {
        return NO_PLAYER;
    }
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
    int line;

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

    if (get_piece_size(game, line, column) != NONE) {
        return EMPTY;
    }

    if (nb_pieces_available(game, piece, player) <= 0) {
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

    if (!are_coordinates_valid(line, column)) {
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

    game->positions_history[0][0] = line;
    game->positions_history[0][1] = column;
    game->history_len = 1;

    return OK;
}

size size_under_picked_piece(board game) {
    return get_piece_size(game, picked_piece_line(game), picked_piece_column(game));
}

bool is_goal_reachable(board game) {
    int movement = game->movement_left;
    
    if (movement == 0) {
        movement = size_under_picked_piece(game); 
    }

    if (game->current_player == NORTH_P && game->picked_piece_line == 0 && movement == 1) {
        return true;
    }

    if (game->current_player == SOUTH_P && game->picked_piece_line == DIMENSION-1 && movement == 1) {
        return true;
    }

    return false;
}

bool is_move_possible(board game, direction testing_direction) {
    int next_line = game->picked_piece_line;
    int next_column = game->picked_piece_column;
    int movement = game->movement_left;
    
    if (movement == 0) {
        movement = size_under_picked_piece(game); 
    }

    if (movement <= 0) {
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

    if (!are_coordinates_valid(next_line, next_column)) {
        return false;
    }

    if (get_piece_size(game, next_line, next_column) != NONE && movement != 1) {
        return false;
    }

    if (déjà venu) {
        return false;
    }

    // TODO :
    //  check if we already entered this position from the same side

    return true;
}

void terminate_move(board game) {
    if (get_winner(game) == NO_PLAYER) {
        game->map[picked_piece_line(game)][picked_piece_column(game)] = picked_piece_size(game);
    }
    game->movement_left = -1;
    game->current_player = NO_PLAYER;
    game->picked_piece_size = NONE;
    game->picked_piece_line = -1;
    game->picked_piece_column = -1;
    game->history_len = 0;
}

return_code move_piece(board game, direction direction) {
    int next_line = game->picked_piece_line;
    int next_column = game->picked_piece_column;

    if (picked_piece_size(game) == NONE) {
        return EMPTY;
    }

    if (direction == GOAL) {
        if (is_goal_reachable(game)) {
            game->winner = game->current_player;
            terminate_move(game);
            return OK;
        } else {
            return FORBIDDEN;
        }
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
        case GOAL: // avoids a warning
            break;
    }

    if (!are_coordinates_valid(next_line, next_column)) {
        return PARAM;
    }

    if (!is_move_possible(game, direction)) {
        return FORBIDDEN;
    }

    if (game->movement_left == 0) {
        game->movement_left = size_under_picked_piece(game);
    }

    game->picked_piece_line = next_line;
    game->picked_piece_column = next_column;
    game->movement_left--;

    if (game->movement_left == 0 && size_under_picked_piece(game) == NONE) {
        terminate_move(game);
        return OK;
    }

    game->positions_history[game->history_len][0] = next_line;
    game->positions_history[game->history_len][1] = next_column;
    game->history_len++;

    return OK;
}

return_code swap_piece(board game, int target_line, int target_column) {
    if (game->movement_left != 0) {
        return EMPTY;
    }
    if (!are_coordinates_valid(target_line, target_column)) {
        return PARAM;
    }
    if (game->map[target_line][target_column] != NONE
            || (target_line == game->picked_piece_line && target_column == game->picked_piece_column)
            || game->current_player == NO_PLAYER) {
        return FORBIDDEN;
    }

    game->map[target_line][target_column] = size_under_picked_piece(game);
    terminate_move(game);
    return OK;
}

return_code cancel_movement(board game) {
    if (game->picked_piece_size == NONE) {
        return EMPTY;
    }

    game->picked_piece_line = game->positions_history[0][0];
    game->picked_piece_column = game->positions_history[0][1];
    terminate_move(game);
    return OK;
}

return_code cancel_step(board game) {
    if (game->picked_piece_size == NONE) {
        return EMPTY;
    }

    if (game->history_len == 1) {
        return cancel_movement(game);
    }

    game->history_len--;
    game->picked_piece_line = game->positions_history[game->history_len-1][0];
    game->picked_piece_column = game->positions_history[game->history_len-1][1];

    if (size_under_picked_piece(game) != NONE) {
        game->movement_left = 0;
    } else {
        game->movement_left++;
    }

    return OK;
}