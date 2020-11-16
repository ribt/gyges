#include <stdio.h>
#include <stdlib.h>
#include "board.h"
#include "display.h"
void firstphase(board game){
	int pawn_size;
	int column;
	int check = 0;
	for(int i = 0; i < DIMENSION; i++){
		while(check == 0){
			printf("C'est au joueur Sud de jouer. Quel pion voulez vous jouer ?\n1/2/3 :");
			scanf("%d", &pawn_size);
			if(nb_pieces_available(game, pawn_size, SOUTH_P) > 0){check = 1;};
			if(check == 0){printf("La pion n'est pas disponible\n");};
		}
		check = 0;
		printf("Il vous reste %d pion(s) de %d.\n",nb_pieces_available(game, pawn_size, SOUTH_P), pawn_size);
		while(check == 0){
			printf("Sur quelle colonne voulez vous jouer ?\n");
			scanf("%d", &column);
			if(place_piece(game, pawn_size, SOUTH_P, column - 1) == OK){check = 1;};
			if(check == 0){printf("La pion n'est pas disponible\n");};
		}
		printf("Le pion de %d a été posé en (%d,1).\n", pawn_size, column);
		next_player(SOUTH_P);
		disp_board(game);
		check = 0;
		while(check == 0){
			printf("C'est au joueur Nord de jouer. Quel pion voulez vous jouer ?\n1/2/3 :");
			scanf("%d", &pawn_size);
			if(nb_pieces_available(game, pawn_size, NORTH_P) > 0){check = 1;};
			if(check == 0){printf("La pion n'est pas disponible\n");};
		}
		check = 0;
		printf("Il vous reste %d pion(s) de %d.\n",nb_pieces_available(game, pawn_size, NORTH_P), pawn_size);
		while(check == 0){
			printf("Sur quelle colonne voulez vous jouer ?\n");
			scanf("%d", &column);
			if(place_piece(game, pawn_size, NORTH_P, column - 1) == OK){check = 1;};
			if(check == 0){printf("La pion n'est pas disponible\n");};
		}
		printf("Le pion de %d a été posé en (%d,1).\n", pawn_size, column);
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
