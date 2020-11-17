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
	int allpawn[NB_SIZE];
	int fisrtplay[DIMENSION];
	for(int k = 0; k < 2; k++){	
		initializetable(allpawn, NB_SIZE, NB_INITIAL_PIECES);
		initializetable(fisrtplay, DIMENSION, 0);
		int pawn_size = 0;
		if(k == 0){	
			printf("Joueur Nord, veuillez saisir de gauche à droite le placement de vos pions\n");
		}else{
			printf("Joueur Sud, veuillez saisir de gauche à droite le placement de vos pions\n");
		}
		for(int i = 0; i < DIMENSION; i++){
			scanf("%d", &pawn_size);
			while (allpawn[pawn_size-1] == 0){
				printf("Il ne vous reste plus de pion de %d.\n", pawn_size);
				for(int j = 0; j < DIMENSION; j++){
					if(fisrtplay[j] > 0){printf("%d\n", fisrtplay[j]);}
				}
				scanf("%d", &pawn_size);
			}
			fisrtplay[i] = pawn_size;
			allpawn[pawn_size-1] -= 1;
			place_piece(game, pawn_size, k+1, i);
		}
		disp_board(game);
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