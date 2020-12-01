#include "board.h"

/* 
This function prints the board, line by line.
    We use BOX DRAWING chars (https://jrgraphix.net/r/Unicode/2500-257F) to display a nice grid pattern.
    Pieces' sizes are in bold and each number has its own color (in bold to be more readable). There are displayed in the middle of the cells.
    The picked piece is displayed at the right of the cell and it is blinking. We use the same colors.
*/
void disp_board(board game);


// This function prints the message in red and bold, preceded by a visible "ERROR" (white on a red background).
void disp_error(char * message);


// This function returns pointer to a string to be displayed using %s with printf.
char * player_name(player this_player);


// This function returns pointer to a string containing "s" if the number is greater than 1 or an empty string.
char * plural(int number);


/*
this function allows us to clear the terminal
we call it very often so for better performances we used a macro function but our teacher didn't like it :c
*/
void clear_screen();