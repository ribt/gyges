#include "bot.h"
#include "board.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "display.h"
#include <time.h>

#define MAX_RANDOM_TRIES 10

#ifdef DEBUG
    #define trace(s) printf(s); usleep(100000);
#else
    #define trace(s)
#endif

level difficulty = HARD;

path NULL_PATH = {
    0,
    {}
};

void set_difficulty(level choice) {
    difficulty = choice;
}

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

void random_piece_placement(board game, player bot) {
    int column = 0;
    while (column < DIMENSION) {
        if (place_piece(game, rand()%3+1, bot, column) == OK) {
            column++;
        }
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
    if (rand()%5 > 0) { // 4/5 = 80% chance to move forward 
        if (bot == NORTH_P) {
            if (is_move_possible(game, SOUTH)) {
                return SOUTH;
            }
        }
        else if (is_move_possible(game, NORTH)) {
            return NORTH;
        }
    }

    if (rand()%10 > 0) { // (1-0.8)*9/10 = 18 %
        if (rand()%2 == 0) { // 0.18*1/2 = 9% to move EAST
            if (is_move_possible(game, EAST)) {
                return EAST;
            }
        } else if (is_move_possible(game, WEST)) { // 9% to move WEST
            return WEST;
        }
    }

    if (bot == NORTH_P) { // 2% chance to move backward
        return NORTH;
    } else {
        return SOUTH;
    }
}

bool can_win(board game, player testing_player);

move random_move(board game, player bot, int tries) {
    move response;
    return_code ret_code;
    direction rand_dir;
    board tmp_game = copy_game(game);

    trace("randome_move\n")

    response.path.len = 0;

    response.piece = random_pickable_piece(tmp_game, bot);
    pick_piece(tmp_game, bot, response.piece.line, response.piece.column);

    while (movement_left(tmp_game) > -1) {
        rand_dir = random_direction(tmp_game, bot);
        ret_code = move_piece(tmp_game, rand_dir);
        if (i_am_blocked(tmp_game, bot) || response.path.len == MAX_PATH_LEN) {
            cancel_movement(tmp_game);
            response.path.len = 0;
            response.piece = random_pickable_piece(tmp_game, bot);
            pick_piece(tmp_game, bot, response.piece.line, response.piece.column);
        } else if (ret_code == OK) {
            response.path.directions[response.path.len] = rand_dir;
            response.path.len++;                
        }
    }
    if (can_win(tmp_game, next_player(bot)) && tries < MAX_RANDOM_TRIES) {
        destroy_game(tmp_game);
        return random_move(game, bot, tries+1);
    }
    destroy_game(tmp_game);
    return response;
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
    int *playable_columns = pickable_pieces(game, testing_player);
    board tmp_game;

    trace("can_win\n")

    while (i <= playable_columns[0]) {
        tmp_game = copy_game(game);
        pick_piece(tmp_game, testing_player, line, playable_columns[i]);
        if (try_to_win(tmp_game)) {
            return 1;
        }
        destroy_game(tmp_game);
        i++;
    }
    free(playable_columns);
    return 0;
}

path win_path(board game, path current_path) {
    path best_path;
    best_path.len = 999;
    board tmp_game;
    path ret_path;
    path tmp_path;

    if (is_move_possible(game, GOAL)) {
        current_path.directions[current_path.len] = GOAL;
        current_path.len++;
        return current_path;
    }

    for (direction dir = SOUTH; dir <= WEST; dir++) { // GOAL, SOUTH, NORTH, EAST, WEST
        if (is_move_possible(game, dir)) {
            tmp_game = copy_game(game);
            move_piece(tmp_game, dir);
            copy_path(&current_path, &tmp_path);
            tmp_path.directions[current_path.len] = dir;
            tmp_path.len++;
            ret_path = win_path(tmp_game, tmp_path);
            if (ret_path.len > 0 && ret_path.len < best_path.len) {
                copy_path(&ret_path, &best_path);
            }
            destroy_game(tmp_game);
        }
    }
    return best_path;
}

move best_move_to_win(board game, player bot) {
    int line = player_line(game, bot);
    int i = 1;
    int *playable_columns = pickable_pieces(game, bot);
    board tmp_game;
    path best_path;
    best_path.len = 999;
    path ret_path;
    position piece;
    piece.line = line;
    move rep;

    while (i <= playable_columns[0]) {
        tmp_game = copy_game(game);
        pick_piece(tmp_game, bot, line, playable_columns[i]);
        ret_path = win_path(tmp_game, NULL_PATH);
        if (ret_path.len > 0 && ret_path.len < best_path.len) {
            copy_path(&ret_path, &best_path);
            piece.column = playable_columns[i];
        }
        destroy_game(tmp_game);
        i++;
    }
    free(playable_columns);
    rep.piece = piece;
    rep.path = best_path;
    return rep;
}

path path_avoiding_enemy_to_win(board game, path current_path, player ennemy) {
    board tmp_game;
    path next_path;
    path ret_path;

    trace("path_avoiding_enemy_to_win\n")

    for (direction dir = SOUTH; dir <= WEST; dir++) { // GOAL, SOUTH, NORTH, EAST, WEST
        if (is_move_possible(game, dir)) {
            tmp_game = copy_game(game);
            move_piece(tmp_game, dir);
            if (movement_left(tmp_game) == -1) {
                if (!can_win(tmp_game, ennemy)) {
                    current_path.directions[current_path.len] = dir;
                    current_path.len++;
                    destroy_game(tmp_game);
                    return current_path;
                }
            } else {
                copy_path(&current_path, &next_path);
                next_path.directions[current_path.len] = dir;
                next_path.len++;
                ret_path = path_avoiding_enemy_to_win(tmp_game, next_path, ennemy);
                if (ret_path.len > 0) {
                    destroy_game(tmp_game);
                    return ret_path;
                }
                destroy_game(tmp_game);
            }
        }
    }
    
    return NULL_PATH;
}

move move_avoiding_enemy_to_win(board game, player bot) {
    int line = player_line(game, bot);
    player ennemy = next_player(bot);
    board tmp_game;
    int *playable_columns = pickable_pieces(game, bot);
    int i = 1;
    path ret_path = NULL_PATH;
    move response;

    while (i <= playable_columns[0] && ret_path.len == 0) {
        tmp_game = copy_game(game);
        pick_piece(tmp_game, bot, line, playable_columns[i]);
        ret_path = path_avoiding_enemy_to_win(tmp_game, NULL_PATH, ennemy);
        if (ret_path.len > 0) {
            response.piece.line = line;
            response.piece.column = playable_columns[i];
            copy_path(&ret_path, &response.path);
            destroy_game(tmp_game);
            return response;
        }
        destroy_game(tmp_game);
        i++;
    }

    return random_move(game, bot, MAX_RANDOM_TRIES);
}

move bot_move(board game, player bot) {
    if (can_win(game, bot) && (difficulty > EASY || rand()%2)) {
        return best_move_to_win(game, bot);
    }

    if (can_win(game, next_player(bot)) && rand()%2 < difficulty) {
        return move_avoiding_enemy_to_win(game, bot);
    }

    return random_move(game, bot, 0);    
}