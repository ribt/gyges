#include <stdio.h>
#include <stdlib.h>
#include "board.h"
#include "display.h"

#define clear_screen() printf("\033[H\033[2J")
#define clear_buffer() while(getchar()!='\n') {}

void initializetable(int table[],int size, int value){
	for(int i = 0; i < size; i++){
		table[i] = value;
	}
}

void firstphase(board game){
	int allpawn[NB_SIZE];	//Allows to check if the pawn is available.
	int historymoves[DIMENSION];	//Temporarily keep the player's moves.

	for (int k = 0; k < NB_PLAYERS; k++) {	//Does the same for both players
		int pawn_size = 0;

		disp_board(game);

		initializetable(allpawn, NB_SIZE, NB_INITIAL_PIECES);
		initializetable(historymoves, DIMENSION, 0);
		
		if (k == 0) {
			printf("Joueur Sud, veuillez saisir un par un, de gauche à droite le placement de vos pions puis valider avec entrée.\n");
		} else {
			printf("Joueur Nord, veuillez saisir un par un,de gauche à droite le placement de vos pions puis valider avec entrée.\n");
		}

		for (int i = 0; i < DIMENSION; i++) {	//To place as many pawns as there are columns
			scanf("%d", &pawn_size);
			clear_buffer();
			while (allpawn[pawn_size-1] == 0 || pawn_size > 3 || pawn_size < 1) {	//Check if the pawn is available.
				clear_screen();
				disp_board(game);

				if (pawn_size > 0 && pawn_size < 4) {
					printf("Il ne vous reste plus de pion de %d.\n", pawn_size);
				} else {
					printf("Ce pion n'existe pas.\n");
				}

				for (int j = 0; j < DIMENSION; j++) {	//Displays all his moves since the beginning of the first phase. 
					if (historymoves[j] > 0) {
						printf("%d ", historymoves[j]);
					}
				}
				scanf("%d", &pawn_size);
				clear_buffer();
			}

			historymoves[i] = pawn_size;	//Updates the history of his moves
			allpawn[pawn_size-1] -= 1;	//Updates moves
			place_piece(game, pawn_size, k+1, i);	//Places the pawn on the board
			clear_screen();
			disp_board(game);

			if (k == 0){
				printf("Joueur Sud, veuillez saisir de gauche à droite le placement de vos pions\n");
			} else {
				printf("Joueur Nord, veuillez saisir de gauche à droite le placement de vos pions\n");
			}

			for (int j = 0; j < DIMENSION; j++) {	//Displays all his moves since the beginning of the first phase. 
					if (historymoves[j] > 0) {
						printf("%d ", historymoves[j]);
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
	firstphase(game);
	printf("Fin du placement des premiers pions, début du jeu\n");
	disp_board(game);	//Displays the board
	destroy_game(game);	
	return 0;	//Deletes the board
}
