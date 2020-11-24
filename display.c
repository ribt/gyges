#include <stdio.h>
#include <unistd.h>
#include "board.h"

#define clear_screen() printf("\033[H\033[2J")

void disp_board(board game) {
    printf("\n                  \033[0;90mNord\n          ╔═══╦═══╦═"); // grey
    if (get_winner(game) == SOUTH_P) {
        printf("\033[1;93m⬤"); // light yellow and bold
    } else {
        printf("\033[0m◯");
    }
    printf("\033[0;90m═╦═══╦═══╗\n        ╭─╨─┬─╨─┬─╨─┬─╨─┬─╨─┬─╨─╮\n");
        
    
    for (int line = DIMENSION-1; line >= 0; line--) {
        printf("        ");
        for (int column = 0; column < DIMENSION; column++) {
            printf("\033[0;90m│ ");            

            switch (get_piece_size(game, line, column)) {
                case NONE: printf(" "); break;
                case ONE: printf("\033[1;34m1"); break;   // blue and bold
                case TWO: printf("\033[1;33m2"); break;   // yellow and bold
                case THREE: printf("\033[1;31m3"); break; // red and bold
            }

            if (line == picked_piece_line(game) && column == picked_piece_column(game)) {
                switch (picked_piece_size(game)) {
                    case ONE: printf("\033[5;1;34m1"); break;   // blue and bold and blink
                    case TWO: printf("\033[5;1;33m2"); break;   // yellow and bold and blink
                    case THREE: printf("\033[5;1;31m3"); break; // red and bold and blink
                }
            } else {
                printf(" ");
            }

            
            if (column == DIMENSION-1) {
                printf("\033[0;90m│");
            }    
        }
        printf("\n");
        if (line == DIMENSION/2) {
            printf(" Ouest \033[0;90m ├───┼───┼───┼───┼───┼───┤  Est\n");
        } else if (line > 0) {
            printf("       \033[0;90m ├───┼───┼───┼───┼───┼───┤\n");
        }
    }

    printf("        \033[0;90m╰─╥─┴─╥─┴─╥─┴─╥─┴─╥─┴─╥─╯\n          ╚═══╩═══╩═");
    if (get_winner(game) == NORTH_P) {
        printf("\033[1;93m⬤"); // yellow and bold
    } else {
        printf("\033[0m◯");
    }
    printf("\033[0;90m═╩═══╩═══╝\n                   Sud\033[0m\n\n"); // \033[0m -> regular text after this print
}

void disp_error(char * message) {
    printf("\033[101mERREUR\033[49m\n\033[1;31m%s\n\033[0m", message);
    // \033[101m -> red background for ERREUR
    // \033[1;31m -> red and bold for the message
    // \033[0m -> regular text after this print
}

char * player_name(player this_player) {
    if (this_player == SOUTH_P) {
        return "\033[1mSud\033[0m";
    }
    if (this_player == NORTH_P) {
        return "\033[1mNord\033[0m";
    }
    return "inconnu";
}

char * plural(int number) {
    if (number > 1) {
        return "s";
    } else {
        return "";
    }
}