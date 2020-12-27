#include "board.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_PATH_LEN 100

void set_map(board game, int map[DIMENSION][DIMENSION]);
bool is_goal_reachable(board game);
void disp_history(board game);
int get_history_len(board game);

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


void copy_path(path *src, path *dst) {
    dst->len = src->len;
    for (int i=0; i<dst->len; i++) {
        dst->directions[i] = src->directions[i];
    }
}

void affichage(board game) {
    for (int l = DIMENSION-1; l >= 0; l--) {
        for (int c = 0; c < DIMENSION; c++) {
            if (l==picked_piece_line(game) && c==picked_piece_column(game)) {
                printf("* ");
            } else {
                printf("%d ", get_piece_size(game, l, c));
            }
        }
        printf("\n");
    }
}


int player_line(board game, player player) {
    if (player == NORTH_P)
        return northmost_occupied_line(game);
    if (player == SOUTH_P)
        return southmost_occupied_line(game);
    return -1;
}

int *pickable_pieces(board game, player player) {
    int *rep = malloc(DIMENSION*sizeof(int));
    int line = player_line(game, player); 
    int i = 0;

    for (int column = 0; column < DIMENSION; column++) {
        if (get_piece_size(game, line, column) != NONE) {
            rep[i] = column;
            i++;
        }
    }
    while (i < DIMENSION) {
        rep[i] = -1;
        i++;
    }
    return rep;
}

char *aff_dir(direction dir) {
    switch (dir) {
        case SOUTH: return "SOUTH";
        case NORTH: return "NORTH";
        case EAST: return "EAST";
        case WEST: return "WEST";
        case GOAL: return "GOAL";
    }

    return "";
}

bool win(board game) {
    board tmp;

    if (picked_piece_line(game)==-1)
        return 0;

    if (is_goal_reachable(game)) {
        //disp_history(game);
        return 1;
    }

    for (int dir = 1; dir < 5; dir++) { // GOAL, SOUTH, NORTH, EAST, WEST
        if (is_move_possible(game, dir)) {
            tmp = copy_game(game);
            move_piece(tmp, dir);
            if (win(tmp))
                return 1;
            destroy_game(tmp);
        }
    }
    return 0;
}



bool can_win(board game, player player) {
    int line = player_line(game, player); 
    int i = 0;
    int *playable = pickable_pieces(game, player);
    board tmp;

    while (i < DIMENSION && playable[i] != -1) {
        tmp = copy_game(game);
        pick_piece(tmp, player, line, playable[i]);
        if (win(tmp)) {
            return 1;
        }
        destroy_game(tmp);
        i++;
    }
    return 0;
}

void disp_path(path path) {
    for (int i = 0; i < path.len; i++) {
        printf("%s ", aff_dir(path.directions[i]));
    }
    printf("\n");
}

path win_path(board game, path current_path) {
    path best_path;
    best_path.len = 999;
    board tmp_board;
    path ret_path;
    path tmp_path;

    printf("win_path(");
    disp_path(current_path);

    if (picked_piece_line(game)==-1)
        return NULL_PATH;

    if (is_goal_reachable(game)) {
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
            if (ret_path.len > 0) {
                if (ret_path.len < best_path.len) {
                    copy_path(&ret_path, &best_path);
                }
            }
            destroy_game(tmp_board);
        }
    }
    return NULL_PATH;
}

path best_path_to_win(board game, player player) {
    int line = player_line(game, player); 
    int i = 0;
    int *playable = pickable_pieces(game, player);
    board tmp_board;
    path best_path;
    best_path.len = 999;
    path tmp_path;

    while (i < DIMENSION && playable[i] != -1) {
        tmp_board = copy_game(game);
        pick_piece(tmp_board, player, line, playable[i]);
        tmp_path = win_path(tmp_board, NULL_PATH);
        if (tmp_path.len > 0) {
            if (tmp_path.len < best_path.len) {
                printf("new min len : %d\n", tmp_path.len);
                copy_path(&tmp_path, &best_path);
            }
        }
        destroy_game(tmp_board);
        i++;
    }
    return best_path;
}



int main() {
    board game = new_game();
    int map[DIMENSION][DIMENSION]= {
        {3, 0, 0, 0, 0, 3},
        {0, 0, 0, 0, 0, 3},
        {0, 0, 1, 0, 2, 0},
        {0, 0, 2, 0, 1, 1},
        {0, 0, 0, 0, 0, 2},
        {1, 0, 2, 0, 0, 0}
    };

    set_map(game, map);

    affichage(game);
    printf("\n");

    if (can_win(game, NORTH_P)) {printf("NORTH can win\n");}
    if (can_win(game, SOUTH_P)) {printf("SOUTH can win\n");}

    printf("\n");

    disp_path(best_path_to_win(game, SOUTH_P));
    

}