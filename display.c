#include <stdio.h>
#include "board.h"

void disp_board(board game) {
    printf("                \033[0;90m(N)\n       ┌───┬───┬\033[0m o \033[0;90m┬───┬───┐\n");
    printf("       │   │   │   │   │   │\n");
    for (int line = DIMENSION-1; line >= 0; line--) {
        printf("      ");
        for (int column = 0; column < DIMENSION; column++) {
            printf("\033[0m ");
            switch (get_piece_size(game, line, column)) {
                case NONE: printf("o"); break;
                case ONE: printf("\033[1;34m1"); break;
                case TWO: printf("\033[1;33m2"); break;
                case THREE: printf("\033[1;31m3"); break;
            }
            if (column < DIMENSION-1) {
                printf(" \033[0;90m─\033[0m");
            }    
        }
        printf("\n");
        if (line > 0) {
            printf("      \033[0;90m |   |   |   |   |   |\n");
        }
    }
    printf("      \033[0;90m │   │   │   │   │   │\n       └───┴───┴\033[0m o \033[0;90m┴───┴───┘\n                (S)\033[0m\n");
}

void disp_error(char * message) {
    printf("\033[101mERREUR\033[49m\n\033[1;31m%s\n\033[0m", message);

}