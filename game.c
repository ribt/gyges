#include <stdio.h>
#include <stdlib.h>
#include "board.h"
#include "display.h"

int initializetable(int table[],int size, int value){
	for(int i = 0; i < size; i++){
		table[i] = value;
	}
}

void firstphase(board game){
	int allpawn[NB_SIZE];		//Allows to check if the pawn is available.
	int historymoves[DIMENSION];	//Temporarily keep the player's moves.
	for(int k = 0; k < 2; k++){	
		initializetable(allpawn, NB_SIZE, NB_INITIAL_PIECES);
		initializetable(historymoves, DIMENSION, 0);
		int pawn_size = 0;
		if(k == 0){	
			printf("Joueur Nord, veuillez saisir de gauche à droite le placement de vos pions\n");
		}else{
			printf("Joueur Sud, veuillez saisir de gauche à droite le placement de vos pions\n");
		}
		for(int i = 0; i < DIMENSION; i++){
			scanf("%d", &pawn_size);
			while (allpawn[pawn_size-1] == 0){		//Check if the pawn is available. 
				printf("Il ne vous reste plus de pion de %d.\n", pawn_size);
				for(int j = 0; j < DIMENSION; j++){	//Displays all his moves since the beginning of the first phase to remind him. 
					if(historymoves[j] > 0){printf("%d\n", historymoves[j]);}
				}
				scanf("%d", &pawn_size);
			}
			historymoves[i] = pawn_size;	//Updates the history of his moves
			allpawn[pawn_size-1] -= 1;	//Updates moves
			place_piece(game, pawn_size, k+1, i);	//Places the pawn on the board
		}
		disp_board(game);	//Displays the board
	}
}


int main(void){
	board game = new_game();//To initialize the board
	printf("Le plateau a été correctement initialisé.\n");
	firstphase(game);
	printf("Fin du placement des premiers pions, début du jeu\n");
	destroy_game(game);
	return 0;
}