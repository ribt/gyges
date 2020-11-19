#include "board.h"

void disp_board(board game);
void disp_error(char * message);
void disp_player_name(player this_player);
return_code disp_movement(board game, player current_player, int line, int column, direction directions[], int nbr_directions);