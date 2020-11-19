#include <stdio.h>
#include "board.h"

void disp_board(board game) {
    printf("               \033[0;90mNord\n       ╔═══╦═══╦═\033[1;93m⬤\033[0;90m═╦═══╦═══╗\n     ┌─╨─┬─╨─┬─╨─┬─╨─┬─╨─┬─╨─┐\n");
    for (int line = DIMENSION-1; line >= 0; line--) {
        printf("     ");
        for (int column = 0; column < DIMENSION; column++) {
            printf("\033[0;90m│ ");
            switch (get_piece_size(game, line, column)) {
                case NONE: printf("  "); break;
                case ONE: printf("\033[1;34m1 "); break;
                case TWO: printf("\033[1;33m2 "); break;
                case THREE: printf("\033[1;31m3 "); break;
            }
            if (column == DIMENSION-1) {
                printf("\033[0;90m│");
            }    
        }
        printf("\n");
        if (line > 0) {
            printf("    \033[0;90m ├───┼───┼───┼───┼───┼───┤\n");
        }
    }
    //printf("     \033[0;90m└─╥─┴─╥─┴──╥┴╥──┴─╥─┴─╥─┘\n       ║   ║   ┌╨─╨┐   ║   ║\n       ╚═══╩═══╡   ╞═══╩═══╝\n               └───┘\n                Sud\033[0m\n");
    printf("     \033[0;90m└─╥─┴─╥─┴─╥─┴─╥─┴─╥─┴─╥─┘\n       ╚═══╩═══╩═\033[1;93m⬤\033[0;90m═╩═══╩═══╝\n                Sud\033[0m\n");
}

void disp_error(char * message) {
    printf("\033[101mERREUR\033[49m\n\033[1;31m%s\n\033[0m", message);
}

void disp_player_name(player this_player) {
    if (this_player == SOUTH_P)
        printf("SUD");
    if (this_player == NORTH_P)
        printf("NORD");
}