#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "board.h"
#include "display.h"

#define clear_screen() printf("\033[H\033[2J")
#define clear_buffer() while(getchar()!='\n') {}

void fill(int tab[], int size, int value) {
	for (int i = 0; i < size; i++) {
		tab[i] = value;
	}
}

void init_game (board game, int *pcurrent_player) {
	int history[DIMENSION];	// temporarily keep the player's choices
	int column, piece_size, res;

	srand(time(NULL));

	if (rand()%2 == 0) {  // random choice of the first player
		*pcurrent_player = NORTH_P;
	} else {
		*pcurrent_player = SOUTH_P;
	}

	for (int i = 0; i < NB_PLAYERS; i++) {	// do the same for all players
		fill(history, DIMENSION, 0);

		clear_screen();
		disp_board(game);
		printf("Joueur %s, veuillez choisir de gauche à droite la taille des pièces à mettre sur votre première ligne.\nValidez avec Entrée pour chaque pièce.\n", player_name(*pcurrent_player));

		column = 0;
		printf("> ");
		while (column < DIMENSION) {  // place the pieces column by column
			piece_size = -1;
			scanf("%d", &piece_size);
			clear_buffer();
			clear_screen();

			res = place_piece(game, piece_size, *pcurrent_player, column); // != EMPTY because we force the choice of the column
			
			if (res == PARAM) {
				disp_error("Cette taille de pion n'existe pas.");
			}
			if (res == FORBIDDEN) {
				disp_error("Il ne vous reste plus de pion de cette taille-là.");
			}
			if (res == OK) {
				history[column] = piece_size;			
				column++;
			}

			disp_board(game);
			printf("Joueur %s, veuillez choisir de gauche à droite la taille des pièces à mettre sur votre première ligne.\nValidez avec Entrée pour chaque pièce.\n> ", player_name(*pcurrent_player));

			for (int j = 0; j < DIMENSION; j++) {	// display all pieces already placed
				if (history[j] > 0) {
					printf("%d ", history[j]);
				}
			}
		}

		*pcurrent_player = next_player(*pcurrent_player);
	}
}

void gameplay(board game, int *pcurrent_player) {
	int line, res;
	int column = -1;
	int available_movments;
	char * agreement;
	char move;
	int check = -1;
	int history[3];
	int i;
	
	while (get_winner(game) == NO_PLAYER) {
		i = 0;
		fill(history, 3, -1);
		if (*pcurrent_player == SOUTH_P) {
			line = southmost_occupied_line(game);
		} else {
			line = northmost_occupied_line(game);
		}

		res = -1;		
		while (res != OK) {
			printf("Joueur %s, choisissez la colonne où prendre la pièce : ", player_name(*pcurrent_player));
			column = -1;
			scanf("%d", &column);
			clear_buffer();
			clear_screen();
			column--;

			res = pick_piece(game, *pcurrent_player, line, column); // != FORBIDDEN because we force the choice of the line
			if (res == EMPTY) {
				disp_error("Il n'y a pas de pièce à cet endroit sur la ligne la plus proche de vous.");
			}
			if (res == PARAM) {
				disp_error("Ce numéro est invalide.");
			}

			disp_board(game);		
		}

		while (movement_left(game) > 0) {
			check = -1;
			move = 0;
			available_movments = movement_left(game);
			agreement = plural(available_movments); // "s" if the number is >1
			while(check != 1){
				while(move != 'N' && move != 'S' && move != 'E' && move != 'O' && move != 'G'){
					printf("%d mouvement%s restant%s\n", available_movments, agreement, agreement);
					printf("Que voulez vous faire ?\n> ");
					for (int j = 0; j < 3; j++) {	// display all pieces already placed
						switch (history[j]) {
							case 2: printf("N "); break;
							case 1: printf("S "); break;
							case 3: printf("E "); break;
							case 4: printf("O "); break;
						};
					}
					scanf("%c", &move);
					clear_buffer();
					clear_screen();
					if(move != 'N' && move != 'S' && move != 'E' && move != 'O' && move != 'G'){
						disp_error("Cette direction n'existe pas.");
					}
					disp_board(game);
				}
				switch (move) {
					case 'N': move = NORTH; break;
					case 'S': move = SOUTH; break;
					case 'E': move = EAST; break;
					case 'O': move = WEST; break;
					case 'G': move = GOAL; break;
				};
				check = is_move_possible(game, move);
				//printf("%d\n", check);
				clear_screen();
				if(check != 1){
					disp_error("Vous ne pouvez pas bouger cette piece dans cette direction.");
				}
				disp_board(game);
			}
			move_piece (game, move);
			clear_screen();
			disp_board(game);
			history[i] = move;
			//printf("%d\n", history[i]);
			i++;
		}


		*pcurrent_player = next_player(*pcurrent_player);
		
	}
}

int main() {
	int current_player;
	board game = new_game();

	#ifdef DEBUG

	place_piece(game, THREE, SOUTH_P, 0);
	place_piece(game, ONE, SOUTH_P, 1);
	place_piece(game, TWO, SOUTH_P, 2);
	place_piece(game, THREE, SOUTH_P, 3);
	place_piece(game, ONE, SOUTH_P, 4);
	place_piece(game, TWO, SOUTH_P, 5);

	place_piece(game, ONE, NORTH_P, 0);
	place_piece(game, ONE, NORTH_P, 1);
	place_piece(game, TWO, NORTH_P, 2);
	place_piece(game, TWO, NORTH_P, 3);
	place_piece(game, THREE, NORTH_P, 4);
	place_piece(game, THREE, NORTH_P, 5);

	current_player = NORTH_P;

	#else

	init_game(game, &current_player);

	#endif

	clear_screen();
	printf("Fin du placement des pièces, début du jeu\n");
	disp_board(game);

	gameplay(game, &current_player);

	destroy_game(game);

	return 0;
}