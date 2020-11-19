#include "board.h"

void disp_board(board game);
void disp_error(char * message);

#define clear_screen() printf("\033[H\033[2J")