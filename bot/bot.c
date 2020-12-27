#include "board.h"
#include <stdio.h>
#include <stdlib.h>

void set_map(board game, int map[DIMENSION][DIMENSION]);
bool is_goal_reachable(board game);
void disp_history(board game);
int get_history_len(board game);

typedef struct {
    int len;
    direction directions[];
} path;

typedef struct {
    int line;
    int column;
} position;


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
}

bool try(board game) {
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
            if (try(tmp))
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
        if (try(tmp)) {
            return 1;
        }
        destroy_game(tmp);
        i++;
    }
    return 0;
}

int main() {
    board game = new_game();
    int map[DIMENSION][DIMENSION]= {
        {3, 0, 0, 0, 0, 3},
        {0, 0, 0, 0, 0, 3},
        {0, 0, 1, 0, 2, 0},
        {0, 0, 2, 0, 1, 1},
        {0, 0, 0, 0, 0, 2},
        {1, 0, 0, 0, 0, 0}
    };

    set_map(game, map);

    affichage(game);
    printf("\n");

    if (can_win(game, NORTH_P)) {printf("NORTH can win\n");}
    if (can_win(game, SOUTH_P)) {printf("SOUTH can win\n");}
    

}