#include <stdio.h>
#include <stdlib.h>
#include "board.h"
#include "display.h"
#define DIMENSION 6
#define NB_SIZE 3
#define NB_PLAYERS 2
#define NB_INITIAL_PIECES 2
void firstphase(board game){
	int pawn = -1;
	int column = -1;
	for(int i = 0; i < 6; i++){
		while(1){
			printf("C'est au joueur Sud de jouer. Quel pion voulez vous jouer ?\n1/2/3 :");
			scanf("%d", &pawn);
			if(nb_pieces_available(game, pawn, SOUTH_P) > 0){break;};
			printf("La pion n'est pas disponible\n");
		}
		printf("Il vous reste %d pion(s) de %d.\n",nb_pieces_available(game, pawn, SOUTH_P), pawn);
		while(1){
			printf("Sur quelle colonne voulez vous jouer ?\n");
			scanf("%d", &column);
			if(place_piece(game, pawn, SOUTH_P, column - 1) == OK){break;};
			printf("La case est déjà occupé ou n'existe pas.\n");
		}
		printf("Le pion de %d a été posé en (%d,1).\n", pawn, column);
		next_player(SOUTH_P);
		disp_board(game);
		while(1){
			printf("C'est au joueur Nord de jouer. Quel pion voulez vous jouer ?\n1/2/3 :");
			scanf("%d", &pawn);
			if(nb_pieces_available(game, pawn, NORTH_P) > 0){break;};
			printf("La pion n'est pas disponible\n");
		}
		printf("Il vous reste %d pion(s) de %d.\n",nb_pieces_available(game, pawn, NORTH_P), pawn);
		while(1){
			printf("Sur quelle colonne voulez vous jouer ?\n");
			scanf("%d", &column);
			if(place_piece(game, pawn, NORTH_P, column - 1) == OK){break;};
			printf("La case est déjà occupé ou n'existe pas.\n");
		}
		printf("Le pion de %d a été posé en (%d,6).\n", pawn, column);
		next_player(NORTH_P);
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
