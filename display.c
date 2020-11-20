#include <stdio.h>
#include <unistd.h>
#include "board.h"

#define clear_screen() printf("\033[H\033[2J")

void disp_board(board game) {
    int piece_size;

    printf("               \033[0;90mNord\n       ╔═══╦═══╦═\033[1;93m⬤\033[0;90m═╦═══╦═══╗\n     ┌─╨─┬─╨─┬─╨─┬─╨─┬─╨─┬─╨─┐\n");
        // \033[0;90m -> grey         \033[1;93m -> light yellow and bold  
    
    for (int line = DIMENSION-1; line >= 0; line--) {
        printf("     ");
        for (int column = 0; column < DIMENSION; column++) {
            printf("\033[0;90m│ ");
            if (line == picked_piece_line(game) && column == picked_piece_column(game)) {
                piece_size = picked_piece_size(game);
            } else {
                piece_size = get_piece_size(game, line, column);
            }

            switch (piece_size) {
                case NONE: printf("  "); break;
                case ONE: printf("\033[1;34m1 "); break;   // blue and bold
                case TWO: printf("\033[1;33m2 "); break;   // yellow and bold
                case THREE: printf("\033[1;31m3 "); break; // red and bold
                default: printf("* ");
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
    printf("     \033[0;90m└─╥─┴─╥─┴─╥─┴─╥─┴─╥─┴─╥─┘\n       ╚═══╩═══╩═\033[1;93m⬤\033[0;90m═╩═══╩═══╝\n                Sud\033[0m\n");
                                                                                              // \033[0m -> regular text after this print
}

void disp_error(char * message) {
    printf("\033[101mERREUR\033[49m\n\033[1;31m%s\n\033[0m", message);
    // \033[101m -> red background for ERREUR
    // \033[1;31m -> red and bold for the message
    // \033[0m -> regular text after this print
}

char * player_name(player this_player) {
    if (this_player == SOUTH_P) {
        return "Sud";
    }
    if (this_player == NORTH_P) {
        return "Nord";
    }
    return "inconnu";
}