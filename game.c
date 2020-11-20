#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "board.h"
#include "display.h"

#define clear_screen() printf("\033[H\033[2J")
#define clear_buffer() while(getchar()!='\n') {}

void fill(int tab[], int size, int value) {
	for(int i = 0; i < size; i++) {
		tab[i] = value;
	}
}

void init_game(board game, int *pcurrent_player){
	int nbr_remaining_p_size[NB_SIZE];	// allow to check if a size still is available
	int history[DIMENSION];	// temporarily keep the player's choices
	int column, piece_size;

	srand(time(NULL));

	if (rand()%2 == 0) {  // random choice of the first player
		*pcurrent_player = NORTH_P;
	} else {
		*pcurrent_player = SOUTH_P;
	}

	for (int i = 0; i < NB_PLAYERS; i++) {	// do the same for all players
		piece_size = -1;

		fill(nbr_remaining_p_size, NB_SIZE, NB_INITIAL_PIECES);
		fill(history, DIMENSION, 0);

		clear_screen();

		disp_board(game);
		
		printf("Joueur %s, veuillez choisir de gauche à droite la taille des pièces à mettre sur votre première ligne.\nValidez avec Entrée pour chaque pièce.\n", player_name(*pcurrent_player));

		column = 0;

		printf("> ");

		while (column < DIMENSION) {  // place the pieces column by column
			scanf("%d", &piece_size);
			clear_buffer();
			clear_screen();
			
			if (piece_size < 1 || piece_size > NB_SIZE) {
				disp_error("Cette taille de pion n'existe pas.");
			}
			else if (nbr_remaining_p_size[piece_size-1] == 0) {
				disp_error("Il ne vous reste pls de pion de cette taille-là.");
			}
			else {
				history[column] = piece_size;
				nbr_remaining_p_size[piece_size-1] -= 1;
				place_piece(game, piece_size, *pcurrent_player, column);
				column++;
			}

			disp_board(game);

			for (int j = 0; j < DIMENSION; j++) {	// display all pieces already placed
				if (history[j] > 0) {
					printf("%d ", history[j]);
				}
			}

		}

		*pcurrent_player = next_player(*pcurrent_player);
	}
}


int main(void){
	int current_player;
	board game = new_game();

	init_game(game, &current_player);

	clear_screen();
	printf("Fin du placement des pièces, début du jeu\n");
	disp_board(game);

	destroy_game(game);

	return 0;
}
