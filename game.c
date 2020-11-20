#include <stdio.h>
#include <stdlib.h>
#include "board.h"
#include "display.h"

#define clear_screen() printf("\033[H\033[2J")
#define clear_buffer() while(getchar()!='\n') {}

void fill_array(int array[], int size, int value){
	for(int i = 0; i < size; i++){
		array[i] = value;
	}
}

void init_board(board game){
	int all_pieces[NB_SIZE];	//Allows to check if the piece is available.
	int moves_history[DIMENSION];	//Temporarily keep the player's moves.

	for (int current_player = 0; current_player < NB_PLAYERS; current_player++) {	//Does the same for both players
		int piece_size = 0;

		disp_board(game);

		fill_array(all_pieces, NB_SIZE, NB_INITIAL_PIECES);
		fill_array(moves_history, DIMENSION, 0);
		
		if (current_player == 0) {
			printf("Joueur Sud, veuillez saisir un par un, de gauche à droite le placement de vos pions puis valider avec entrée.\n");
		} else {
			printf("Joueur Nord, veuillez saisir un par un,de gauche à droite le placement de vos pions puis valider avec entrée.\n");
		}

		for (int i = 0; i < DIMENSION; i++) {	//To place as many pieces as there are columns
			scanf("%d", &piece_size);
			clear_buffer();
			while (all_pieces[piece_size-1] == 0 || piece_size > 3 || piece_size < 1) {	//Check if the piece is available.
				clear_screen();
				disp_board(game);

				if (piece_size > 0 && piece_size < 4) {
					printf("Il ne vous reste plus de pion de %d.\n", piece_size);
				} else {
					printf("Ce pion n'existe pas.\n");
				}

				for (int j = 0; j < DIMENSION; j++) {	//Displays all his moves since the beginning of the first phase. 
					if (moves_history[j] > 0) {
						printf("%d ", moves_history[j]);
					}
				}
				scanf("%d", &piece_size);
				clear_buffer();
			}

			moves_history[i] = piece_size;	//Updates the history of his moves
			all_pieces[piece_size-1] -= 1;	//Updates moves
			place_piece(game, piece_size, k+1, i);	//Places the piece on the board
			clear_screen();
			disp_board(game);

			if (current_player == 0){
				printf("Joueur Sud, veuillez saisir de gauche à droite le placement de vos pions\n");
			} else {
				printf("Joueur Nord, veuillez saisir de gauche à droite le placement de vos pions\n");
			}

			for (int j = 0; j < DIMENSION; j++) {	//Displays all his moves since the beginning of the first phase. 
				if (moves_history[j] > 0) {
					printf("%d ", moves_history[j]);
				}
			}
		}
		clear_screen();
	}
}


int main(void){
	board game = new_game();	//To initialize the board
	clear_screen();
	printf("Le plateau a été correctement initialisé :\n");
	init_board(game);
	printf("Fin du placement des premiers pions, début du jeu\n");
	disp_board(game);	//Displays the board
	destroy_game(game);	
	return 0;	//Deletes the board
}
