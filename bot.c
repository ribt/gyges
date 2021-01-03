#include "board.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "display.h"
#include <time.h>

#define MAX_PATH_LEN 100
#define PAUSE_MS 750

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
    disp_board(game);
    #ifndef DEBUG
        usleep(PAUSE_MS*1000);
    #endif
    pick_piece(game, bot, move.piece.line, move.piece.column);
    for (int i = 0; i < move.path.len; i++) {
        clear_screen();
        disp_board(game);
        usleep(PAUSE_MS*1000);
        move_piece(game, move.path.directions[i]);
    }
}

position random_pickable_piece(board game, player bot) {
    position rep;
    int *pickable = pickable_pieces(game, bot);

    rep.line = player_line(game, bot);
    rep.column = pickable[rand()%pickable[0] + 1];

    return rep;
}

bool i_am_blocked(board game, player bot) {
    if (movement_left(game) == -1) {
        return false;
    }
    for (int dir = SOUTH; dir <= WEST; dir++) {
        if (is_move_possible(game, dir)) {
            return false;
        }
    }
    return true;
}

// better choice than random -> try to approach the goal case
direction random_direction(board game, player bot) {
    if (rand()%4 > 0) { // 75% chance to move forward 
        if (bot == NORTH_P) {
            if (is_move_possible(game, SOUTH)) {
                return SOUTH;
            }
        }
        else if (is_move_possible(game, NORTH)) {
            return NORTH;
        }
    }

    if (rand()%5 > 0) { // 25%*80% = 20 %
        if (rand()%2 == 0) { // 20%*50% = 10% to move EAST
            if (is_move_possible(game, EAST)) {
                return EAST;
            }
        } else if (is_move_possible(game, WEST)) { // 10% to move WEST
            return WEST;
        }
    }

    if (bot == NORTH_P) { // 5% chance to move backward
        return NORTH;
    } else {
        return SOUTH;
    }
}

move random_move(board game, player bot) {
    move rep;
    return_code response;
    direction rand_dir;
    board tmp_game = copy_game(game);

    rep.path.len = 0;

    rep.piece = random_pickable_piece(tmp_game, bot);
    pick_piece(tmp_game, bot, rep.piece.line, rep.piece.column);

    while (movement_left(tmp_game) > -1) {
        rand_dir = random_direction(tmp_game, bot);
        printf("rand_dir: %s\n", dir_string(rand_dir));
        response = move_piece(tmp_game, rand_dir);
        if (i_am_blocked(tmp_game, bot) || rep.path.len == MAX_PATH_LEN) {
            cancel_movement(tmp_game);
            rep.path.len = 0;
            rep.piece = random_pickable_piece(tmp_game, bot);
            pick_piece(tmp_game, bot, rep.piece.line, rep.piece.column);
        } else if (response == OK) {
            rep.path.directions[rep.path.len] = rand_dir;
            rep.path.len++;                
        }
    }
    destroy_game(tmp_game);
    return rep;
}

void bot_move(board game, player bot) {
    if (can_win(game, bot)) {
        disp_move(game, bot, best_move_to_win(game, bot));
    } else {
        disp_move(game, bot, random_move(game, bot));
    }
    
}