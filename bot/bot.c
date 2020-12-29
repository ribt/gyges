#include "board.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "display.h"
#include <time.h>

#define MAX_PATH_LEN 100
#define PAUSE_SEC 1
#define DEBUG 0 

typedef struct {
    int len;
    direction directions[MAX_PATH_LEN];
} path;

path NULL_PATH = {
    0,
    {}
};

typedef struct {
    int line;
    int column;
} position;

typedef struct {
    position piece;
    path path;
} move;


void copy_path(path *src, path *dst) {
    dst->len = src->len;
    for (int i = 0; i < dst->len; i++) {
        dst->directions[i] = src->directions[i];
    }
}


int player_line(board game, player bot) {
    if (bot == NORTH_P) {
        return northmost_occupied_line(game);
    }

    if (bot == SOUTH_P) {
        return southmost_occupied_line(game);
    }

    return -1;
}

// the first int is the size of the list
int *pickable_pieces(board game, player bot) {
    int *rep = malloc((DIMENSION+1)*sizeof(int));
    int line = player_line(game, bot); 
    int i = 1;

    for (int column = 0; column < DIMENSION; column++) {
        if (get_piece_size(game, line, column) != NONE) {
            rep[i] = column;
            i++;
        }
    }

    rep[0] = i-1;
    return rep;
}

char *dir_string(direction dir) {
    switch (dir) {
        case SOUTH: return "SOUTH";
        case NORTH: return "NORTH";
        case EAST: return "EAST";
        case WEST: return "WEST";
        case GOAL: return "GOAL";
    }

    return "";
}

void random_piece_placement(board game, player bot) {
    int column = 0;
    srand(time(NULL));
    while (column < DIMENSION) {
        if (place_piece(game, rand()%3+1, bot, column) == OK) {
            column++;
        }
    }
}

bool try_to_win(board game) {
    board tmp;

    if (is_move_possible(game, GOAL)) {
        return 1;
    }

    for (direction dir = SOUTH; dir <= WEST; dir++) { // GOAL, SOUTH, NORTH, EAST, WEST
        if (is_move_possible(game, dir)) {
            tmp = copy_game(game);
            move_piece(tmp, dir);
            if (movement_left(tmp) > -1 && try_to_win(tmp)) {
                return 1;
            }
            destroy_game(tmp);
        }
    }
    return 0;
}

bool can_win(board game, player testing_player) {
    int line = player_line(game, testing_player); 
    int i = 1;
    int *playable = pickable_pieces(game, testing_player);
    board tmp;

    while (i <= playable[0]) {
        tmp = copy_game(game);
        pick_piece(tmp, testing_player, line, playable[i]);
        if (try_to_win(tmp)) {
            return 1;
        }
        destroy_game(tmp);
        i++;
    }
    free(playable);
    return 0;
}

path win_path(board game, path current_path) {
    path best_path;
    best_path.len = 999;
    board tmp_board;
    path ret_path;
    path tmp_path;

    if (is_move_possible(game, GOAL)) {
        current_path.directions[current_path.len] = GOAL;
        current_path.len++;
        return current_path;
    }

    for (direction dir = SOUTH; dir <= WEST; dir++) { // GOAL, SOUTH, NORTH, EAST, WEST
        if (is_move_possible(game, dir)) {
            tmp_board = copy_game(game);
            move_piece(tmp_board, dir);
            copy_path(&current_path, &tmp_path);
            tmp_path.directions[current_path.len] = dir;
            tmp_path.len++;
            ret_path = win_path(tmp_board, tmp_path);
            if (ret_path.len > 0 && ret_path.len < best_path.len) {
                copy_path(&ret_path, &best_path);
            }
            destroy_game(tmp_board);
        }
    }
    return best_path;
}

move best_move_to_win(board game, player bot) {
    int line = player_line(game, bot); 
    int i = 1;
    int *playable = pickable_pieces(game, bot);
    board tmp_board;
    path best_path;
    best_path.len = 999;
    path ret_path;
    position piece;
    piece.line = line;
    move rep;

    while (i <= playable[0]) {
        tmp_board = copy_game(game);
        pick_piece(tmp_board, bot, line, playable[i]);
        ret_path = win_path(tmp_board, NULL_PATH);
        if (ret_path.len > 0 && ret_path.len < best_path.len) {
            copy_path(&ret_path, &best_path);
            piece.column = playable[i];
        }
        destroy_game(tmp_board);
        i++;
    }
    free(playable);
    rep.piece = piece;
    rep.path = best_path;
    return rep;
}

// execute and display a move
void disp_move(board game, player bot, move move) {
    pick_piece(game, bot, move.piece.line, move.piece.column);
    disp_board(game);
    for (int i = 0; i < move.path.len; i++) {
        if (!DEBUG) {
            sleep(PAUSE_SEC);
            clear_screen();
        }
        move_piece(game, move.path.directions[i]);
        disp_board(game);        
    }
}

void pick_random_piece(board game, player bot) {
    while (picked_piece_size(game) == NONE) {
        pick_piece(game, bot, player_line(game, bot), rand()%DIMENSION);
    }
}

void disp_random_move(board game, player bot) {
    pick_random_piece(game, bot);

    while (movement_left(game) > -1) {
        if (move_piece(game, rand()%4+1) == OK) {  // GOAL, SOUTH, NORTH, EAST, WEST
            if (!DEBUG) {
                sleep(PAUSE_SEC);
                clear_screen();
            }
            disp_board(game);
                
        }
    }
}

void bot_move(board game, player bot) {
    if (can_win(game, bot)) {
        disp_move(game, bot, best_move_to_win(game, bot));
    } else {
        disp_random_move(game, bot);
    }
    
}