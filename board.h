#ifndef _BOARD_H_
#define _BOARD_H_

#include <stdbool.h>

/**
 * \file board.h
 *
 * \brief Gyges game engine functions.
 *
 * In this file, all the functions for having a game of gyges
 * run are defined.
 *  
 * \author Dorbec
 *
 */

/**
 * \mainpage Project 2020-2021 for IUT Grand Ouest Normandie, 
 * computer science department.
 *
 * \section description Description of the project
 * This project is aimed to implement a two players board game, 
 * called gyges. 
 * The rules of the game are described below.
 * 
 * The project is divided into various files. Mostly, the engine of the game,
 * implementing the detailed rules, is provided in the files board.c and board.h
 *
 * \section rules Detailed rules of the game.
 * 
 * The game is played by two players on a six by six squares board, 
 * with two goals. Detailed rules are included in the pdf subject, not recalled here (yet).
 */

/**
 * @brief Game dimension.
 *
 * For being general in terms of dimensions,
 * they are defined in a general parameter,
 * though the parameter is not supposed to change.
 * In the following, all indices are given from 0 to DIMENSION - 1.
 * Small line numbers correspond to south.
 */
#define DIMENSION 6

/**
 * @brief Pointer to the structure that holds the game. 
 *
 * Details of the content are not necessary for its use, so the structure is not 
 * included here.
 */
typedef struct board_s* board;

/**
 * @brief the different sizes of pieces.
 *
 * This set includes reference to an 'empty' size, for representing no piece at all. 
 * They are ordered by increasing size, so that they can be compared with < or >.
 */
enum sizes_e {
	/// no size, for representing no piece
	NONE, /// size 1
	ONE, /// size 2
	TWO, /// size 3
	THREE
	}; 
typedef enum sizes_e size;
/*
 * @brief the different sizes of pieces.
 *
 * This also includes a empty size for having a same type when there is no piece 
 * in a place.
 */
//typedef enum sizes_e size;

/**
 * @brief number of sizes for robustness if the enumeration is modified.
 */
#define NB_SIZE 3

/**
 * @brief the different players for further reference. 
 * NO_PLAYER is used when informing that a square is empty.
 * SOUTH starts on the side of the board with line number 0, 
 * NORTH on the side with line number DIMENSION + 1
 */
enum players_e {NO_PLAYER, SOUTH_P, NORTH_P};

typedef enum players_e player;


/**
 * @brief the different directions in the game.
 * GOAL is legal only on the northmost line for south player, southmost line for north player.
 * other directions are natural.
 */
enum direction_e {GOAL, SOUTH, NORTH, EAST, WEST};

typedef enum direction_e direction;

/**
 * @brief number of players in the game.
 */
#define NB_PLAYERS 2

/**
 * @brief return the next player
 * 
 * This function simply return the player following current_player in the game turn.
 * It does not use any information from the board game it is merely a turn among the players.
 * It also serve as an example for the unit tests. 
 *
 * @param player the player to change
 * @return the next player
 */
player next_player(player current_player);

/**
 * @brief number of pieces of each size on each player's line at the beginning.
 * Usually, this value is 2.
 */
#define NB_INITIAL_PIECES 2

/**
 * @brief error codes returned by the function
 */

enum return_code_e {
	///success
	OK,
	/// given space should or should not be empty
	EMPTY,
	/// forbidden request
	FORBIDDEN,
/*	/// invalid position on the board
	POSITION,*/
	/// invalid parameter
	PARAM
	};
typedef enum return_code_e return_code;


/**@{
 * \name Creation/deletion functionalities.
 */

/**
 * @brief Defines a new empty board for starting a game of gobblet-gobblers.
 */
board new_game();

/**
 * @brief Makes a deep copy of the game.
 * @param original_game the game to copy.
 * @return a new copy fully independent of the original game.
 */
board copy_game(board original_game);

/**
 * @brief Delete the game and frees all required memory.
 * @param game the game to destroy.
 */
void destroy_game(board game);

/**@}*/

/**@{
 * \name Accessing game data functionalities.
 */

/**
 * @brief returns the size of the piece at the precised position on the board, if any.
 * 
 * Return the size of a piece on the board, NONE if there is no piece.
 * If there is a piece being moved on that place, it is not taken into account by this function.
 * If the coordinates do not correspond to a valid place on the board, returns NONE.
 *
 * @param game the game from which to collect information.
 * @param line the line number, South has small values.
 * @param column the column number.
 * @return the size of the holding piece.
 */
size get_piece_size(board game, int line, int column);

/**
 * @brief Tells if the game has a winner
 *
 * Recall that winning condition is that one player reached the goal, placing one piece there.
 *
 * @param game the game to test.
 * @return the player who wins, possibly NO_PLAYER.
 */
player get_winner(board game);

/**
 * @brief Returns the most southern line number which contains a piece
 * 
 * This can be used for deciding whether a south player move is legal
 * This is the smallest line number to carry a piece that can move for player South.
 * If there is no piece that can move on any line, return -1 (the board is empty).
 *
 * @param game the game to consider.
 * @return the line number.
 **/
int southmost_occupied_line(board game);

/**
 * @brief Returns the northmost line number which contains a piece.
 * 
 * This can be used for deciding whether a north player move is legal.
 * This is the largest line number to carry a piece that can move for player North.
 * If there is no piece that can move on any line, return -1 (the board is empty)
 *
 * @param game the game to consider.
 * @return the line number.
 **/
int northmost_occupied_line(board game);


/**
 * @brief returns the player whose piece is currently moving
 * 
 * If a player is currently in a move, returns that player's side.
 * Otherwise, returns NO_PLAYER.
 *
 * @param game the game to consider.
 * @return the player who is currently moving.
 */
player picked_piece_owner(board game);

/**
 * @brief returns the size of the piece currently moving (if any)
 *
 * If a player is currently in a move,
 * returns the piece size, 
 * returns NONE if there is no piece currently in hand.
 * 
 * @param game the game to consider.
 * @return the size of the piece currently moving.
 */
size picked_piece_size(board game);

/**
 * @brief returns the line number of the piece currently moving (if any).
 * 
 * If a player is currently moving, return the line number of the moving piece.
 * returns -1 if no piece is being moved.
 *
 * @param game the game to consider.
 * @return the line number of the moving piece.
 */
int picked_piece_line(board game);

/**
 * @brief returns the column number of the piece currently moving (if any)
 * 
 * If a player is currently moving, return the column number of the moving piece.
 * Returns -1 if no piece is being moved.
 *
 * @param game the game to consider.
 * @return the column number of the moving piece.
 */
int picked_piece_column(board game);

/**
 * @brief returns the number of movement units left to the current moving piece
 * 
 * If a piece is currently being moved, this function returns the number of movement
 * unit left to that piece (ignoring future bouncing)
 * This function returns 0 if and only if the piece just finished its movement over another piece
 * (i.e. the player must choose whether to bounce or to substitute the piece with the one under.) 
 * If there is no piece under movement, this function returns -1.
 * 
 * @param game the game to consider.
 * @return the number of moves left to the piece.
 */
int movement_left(board game);

/**@}*/

/**@{
 * \name Game setting up
 * 
 * Functionalities for the initial phase, initial placing of the pieces.
 */

/**
 * @brief Indicates whether the size of piece is still to be placed by the suggested player
 *
 * returns the number of pieces of the given size the players still need to place, 
 * -1 if the parameters are invalid.
 * The behaviour of this function simply assumes the game is in initial phase, no verification is made.
 * 
 * @param game the game to be considered
 * @param piece the piece size requested
 * @param player the player whose piece is to check
 * @return the number of pieces of that size to be placed by the player
 */
int nb_pieces_available(board game, size piece, player player);

/**
 * @brief places a piece on the board, during the initial setting up of the game. 
 * 
 * The piece is placed on the given player's side, which determines the line number.
 * If placing the piece is not possible, the function returns in this order:
 * - PARAM: if the parameters are invalid (wrong player, piece size or position out of the board)
 * - EMPTY: the indicated position is not empty.
 * - FORBIDDEN: this player has already placed his maximum number of pieces of that size.
 * 
 * @param game the game to consider
 * @param piece the piece size requested
 * @param player the player whose piece to place
 * @param column the column where to place the piece (from 0 to DIMENSION - 1)
 * @return a return code stating the result of the placement
 */
return_code place_piece(board game, size piece, player player, int column);



/**@}*/

/**@{
 * \name Playing functionalities
 */

/**
 * @brief Select a piece to start with.
 *
 * The piece must be taken from the player's line. 
 * Whether the player is allowed to play the piece is tested. 
 * The piece is then ready to be moved with the other functions, 
 * 
 * If picking the piece is not possible, the return_code indicates why, 
 * in this order:
 * - PARAM: invalid parameters.
 * - EMPTY: the position is empty.
 * - FORBIDDEN: the piece does not belong to the line the player is supposed to move from,
 *    or the game already has a winner.
 * Otherwise, returns OK for success.
 *
 * @param game the game where to pick a piece.
 * @param current_player the player who is supposed to be playing.
 * @param line the line number of where to pick the piece (from 0 to DIMENSION - 1).
 * @param column the column number of where to pick the piece (from 0 to DIMENSION - 1).
 * @return an integer encoding the result of the placement, 0 if things went smoothly.
 */
return_code pick_piece(board game, player current_player, int line, int column);

/**
 * @brief Indicates whether a move is possible.
 *
 * Returns true if the move is possible for the current piece.
 * The direction GOAL is possible if the piece is on the northmost
 * line and south is playing, or reversely.
 * Other moves are possible if the place indicated by the direction, or
 * if it contains a piece and the piece has only one movement unit left.
 * Additionnally, if the piece has already made a move along this line, 
 * the movement should no longer be possible.
 *
 * @param game the game where to move a piece.
 * @param direction the direction where the movement is aimed 
 * @return whether a move in that direction is possible.
 */
bool is_move_possible(board game, direction direction);


/**
 * @brief Moves the current picked piece, if possible.
 *
 * Moves the piece according to the given direction.
 * If the piece arrived over another piece at the end of its previous movement,
 * calling this function set that the piece bounced and continues its movement.
 * The direction may be GOAL if the piece is on the closest line to the goal.
 * If moving the piece is not possible, returns a return_code encoding the 
 * identified problem.
 *   EMPTY: there is no piece currently in the players hand.
 *   PARAM: the target position is off the board
 *   FORBIDDEN: moving toward that position is not allowed 
 *      (e.g. occupied while the piece movement is not finished 
 * 	 or target is GOAL but not reachable)
 * Otherwise, returns OK for success.
 *
 * @param game the game where to move a piece.
 * @param direction the direction where the movement is aimed.
 * @return a return_code encoding the result of the placement.
 */
return_code move_piece(board game, direction direction);

/**
 * @brief swap the current piece with the piece below it, and place the piece below at the given position.
 * 
 * If the piece moving just finished its movement over another piece, 
 * this function allows the player to swap the two pieces, and place the piece below anywhere.
 * A successful swap finishes the movement of the player (no picked piece, no moves left, etc.)
 * The function returns OK when the operation succeeds, 
 * otherwise a return_code corresponding the the problem met:
 * - EMPTY if the swapping is not possible at the moment 
 *      (the current moving piece has not finished its movement over another piece)
 * - PARAM when the position proposed is off the board.
 * - FORBIDDEN when the position proposed is not free or the move is not currently possible.
 * @param game the game where to move a piece.
 * @param target_line the line number of where to move the swapped piece.
 * @param target_column the column number of where to move the swapped piece.
 * @return a return code encoding the result of swap, OK if things went smoothly.
 */
return_code swap_piece(board game, int target_line, int target_column);

/**
 * @brief cancels the current movement of the player, putting the piece back where it started.
 * 
 * This is necessary since a player could start a movement with some piece which have no possible issue.
 * Using this function, the moving piece is placed back where it started, and no piece is consiedered picked anymore.
 * The function returns OK when the operation succeeds, 
 * otherwise a return_code corresponding the the problem met:
 * - EMPTY if there is no piece currently in movement.
 * 
 * @param game the game where to cancel the movement.
 * @return a return code encoding the result of swap, OK if things went smoothly.
 */
return_code cancel_movement(board game);

/**
 * @brief cancels the last step of the current move.
 *
 * This function cancels only the last step of the current move. 
 * If the last step was just picking the piece, this is equivalent to cancelling the movement.
 * 
 * The function returns OK when the operation succeeds, 
 * otherwise a return_code corresponding the the problem met:
 * - EMPTY if there is no piece currently in movement.
 *
 * @param game the game where to cancel the movement.
 * @return a return code encoding the result of swap, OK if things went smoothly.
 */
return_code cancel_step(board game);


/**@}*/

#endif /*_BOARD_H_*/
