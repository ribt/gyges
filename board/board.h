#ifndef _BOARD_H_
#define _BOARD_H_

#include <stdbool.h>



#define DIMENSION 6

typedef struct board_s* board;

enum sizes_e {
	/// no size, for representing no piece
	NONE, /// size 1
	ONE, /// size 2
	TWO, /// size 3
	THREE
	}; 
typedef enum sizes_e size;
//typedef enum sizes_e size;

#define NB_SIZE 3

enum players_e {NO_PLAYER, SOUTH_P, NORTH_P};

typedef enum players_e player;


enum direction_e {GOAL, SOUTH, NORTH, EAST, WEST};

typedef enum direction_e direction;

#define NB_PLAYERS 2

player next_player(player current_player);

#define NB_INITIAL_PIECES 2


enum return_code_e {
	///success
	OK,
	/// given space should or should not be empty
	EMPTY,
	/// forbidden request
	FORBIDDEN,
	/// invalid parameter
	PARAM
	};
typedef enum return_code_e return_code;



board new_game();

board copy_game(board original_game);

void destroy_game(board game);



size get_piece_size(board game, int line, int column);

player get_winner(board game);

int southmost_occupied_line(board game);

int northmost_occupied_line(board game);


player picked_piece_owner(board game);

size picked_piece_size(board game);

int picked_piece_line(board game);

int picked_piece_column(board game);

int movement_left(board game);



int nb_pieces_available(board game, size piece, player player);

return_code place_piece(board game, size piece, player player, int column);





return_code pick_piece(board game, player current_player, int line, int column);

bool is_move_possible(board game, direction direction);


return_code move_piece(board game, direction direction);

return_code swap_piece(board game, int target_line, int target_column);

return_code cancel_movement(board game);

return_code cancel_step(board game);



#endif 