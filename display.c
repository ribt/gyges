#include <stdio.h>
#include <unistd.h>
#include "board.h"
#include "bot.h"

#define PAUSE_MS 750

// the comments describing the functions are in display.h

void clear_screen() {
    printf("\033[H\033[2J");
}


void disp_board(board game) {
    /*
                  Nord
          ╔═══╦═══╦═◯═╦═══╦═══╗
        ╭─╨─┬─╨─┬─╨─┬─╨─┬─╨─┬─╨─╮
        │   │   │   │   │   │   │
        ├───┼───┼───┼───┼───┼───┤
        │   │   │   │   │   │   │
        ├───┼───┼───┼───┼───┼───┤
        │   │   │   │   │   │   │
 Ouest  ├───┼───┼───┼───┼───┼───┤  Est
        │   │   │   │   │   │   │
        ├───┼───┼───┼───┼───┼───┤
        │   │   │   │   │   │   │
        ├───┼───┼───┼───┼───┼───┤
        │   │   │   │   │   │   │
        ╰─╥─┴─╥─┴─╥─┴─╥─┴─╥─┴─╥─╯
          ╚═══╩═══╩═◯═╩═══╩═══╝
                   Sud
*/
    printf("\n                  \033[0;90mNord\n          ╔═══╦═══╦═"); // grey

    if (get_winner(game) == SOUTH_P) {
        printf("\033[1;93m⬤"); // light yellow and bold
    } else {
        printf("\033[0m◯");
    }

    printf("\033[0;90m═╦═══╦═══╗\n        ╭─╨─┬─╨─┬─╨─┬─╨─┬─╨─┬─╨─╮\n");
        
    
    for (int line = DIMENSION-1; line >= 0; line--) { // line n°0 is the South
        printf("        ");
        for (int column = 0; column < DIMENSION; column++) {
            printf("\033[0;90m│ "); // space at the begining of a cell   

            switch (get_piece_size(game, line, column)) {
                case NONE: printf(" "); break;
                case ONE: printf("\033[1;34m1"); break;   // blue and bold
                case TWO: printf("\033[1;33m2"); break;   // yellow and bold
                case THREE: printf("\033[1;31m3"); break; // red and bold
            }

            if (line == picked_piece_line(game) && column == picked_piece_column(game)) { // print the picked piece
                printf("\033[1;"); // bold
                #ifndef DEBUG
                    printf("5;"); // blinking
                #endif
                switch (picked_piece_size(game)) {
                    case NONE: break; // never happens but it avoids a warning
                    case ONE: printf("34m1"); break;   // blue
                    case TWO: printf("33m2"); break;   // yellow
                    case THREE: printf("31m3"); break; // red
                }
            } else {
                printf(" "); // complete the cell to be 3 chars long if there is no picked piece
            }

            if (column == DIMENSION-1) { // close the grid
                printf("\033[0;90m│");
            }    
        }
        printf("\n");
        if (line == DIMENSION/2) { // the middle line is different
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
        return "\033[1mSUD\033[0m"; // bold
    }
    if (this_player == NORTH_P) {
        return "\033[1mNORD\033[0m"; // bold
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

// for the bot: execute and display a move
void disp_move(board game, player bot, move move) {
    #ifdef DEBUG
        clear_screen();
    #endif
    disp_board(game);
    usleep(PAUSE_MS*1000);
    pick_piece(game, bot, move.piece.line, move.piece.column);
    for (int i = 0; i < move.path.len; i++) {
        clear_screen();
        disp_board(game);
        usleep(PAUSE_MS*1000);
        move_piece(game, move.path.directions[i]);
    }
    clear_screen();
    disp_board(game);
}