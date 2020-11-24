#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "board.h"
#include "display.h"

#define clear_screen() printf("\033[H\033[2J")
#define clear_buffer() while(getchar()!='\n') {}

void fill(int array[], int size, int value) {
	for (int i = 0; i < size; i++) {
		array[i] = value;
	}
}

void init_game(board game, int *pcurrent_player) {
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
	int line;
	int column = -1;
	int available_movments;
	char * agreement;
	char input;
	direction dir_input;
	int res;
	int swapping_possible;
	char history[100];
	int input_is_correct;
	int size_under_picked_piece;
	char next_char;
	
	while (get_winner(game) == NO_PLAYER) {
		history[0] = '\0'; // history = ""

		if (*pcurrent_player == SOUTH_P) {
			line = southmost_occupied_line(game);
		} else {
			line = northmost_occupied_line(game);
		}

		res = -1;		
		while (res != OK) {
			disp_board(game);

			printf("Joueur %s, choisissez la colonne où prendre votre pièce (1-6) : ", player_name(*pcurrent_player));
			column = -1; // because scanf doesn't edit the variable if we don't enter a number
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
		}

		available_movments = movement_left(game);
		while (available_movments != -1) {	
			agreement = plural(available_movments); // "s" if the number is >1

			input_is_correct = 0;
			while (!input_is_correct) {
				disp_board(game);

				if (available_movments == 0) {
					size_under_picked_piece = get_piece_size(game, picked_piece_line(game), picked_piece_column(game));
					printf("Vous êtes sur une pièce de taille %d. Vous avez le choix entre :\n", size_under_picked_piece);
					printf("- rebondir de %d case%s : entrez de nouveaux points cardinaux pour vous déplacer\n", size_under_picked_piece, plural(size_under_picked_piece));
					printf("- prendre sa place et la placer ailleurs sur le plateau en faisant P\n> ");
					swapping_possible = 1;

				} else {
					printf("Déplacez-vous en entrant des points cardinaux (N, S, E, O).\nSi vous êtes sur la dernière ligne, faites G pour gagner.\nFaites A pour annuler votre dernier coup.\n\n");
					printf("%d mouvement%s restant%s\n> ", available_movments, agreement, agreement);
				}

				printf("%s", history);
				scanf("%c", &input);
				clear_buffer();
				clear_screen();

				if (input == 'P') {
					if (swapping_possible == 0) {
						disp_error("Cette direction n'existe pas.");
					} else {
						input_is_correct = 1;
					}
				} else if (input == 'N' || input == 'S' || input == 'E' || input == 'O' || input == 'G' || input == 'A') {
					input_is_correct = 1;
				} else {
					disp_error("Cette direction n'existe pas.");
					
				}

			} // input is now a correct char

			if (input == 'A') {
				cancel_step(game); // == OK because a piece is picked

				history[strlen(history)-2] = '\0'; // remove the last 2 characters

			} else if (input == 'P') {
				clear_screen();
				res = -1;

				while (res != OK) {
					disp_board(game);
					printf("À tout moment, vous pouvez entrer A pour revenir à l'écran précédent.\n\nChoisissez un nouvel emplacement pour la pièce de taille %d.\n", size_under_picked_piece);
					line = -1;
					printf("Entrez le n° de ligne, 1 étant la ligne Sud (1-6) : ");
					scanf("%d", &line);
					next_char = getchar();
					if (next_char == 'A' && getchar() == '\n') {
						res = OK;
					} else if (next_char != '\n') {
						clear_buffer();
					}
					line--;

					printf("Entrez le n° de colonne (1-6) : ");
					column = -1;
					scanf("%d", &column);
					next_char = getchar();
					if (next_char == 'A' && getchar() == '\n') {
						res = OK;
					} else if (next_char != '\n') {
						clear_buffer();
					}
					column--;

					clear_screen();

					if (res != OK) {
						res = swap_piece(game, line, column);

						if (res == PARAM) {
							disp_error("Vous n'avez pas entré des numéros corrects.");
						}
						if (res == EMPTY) {
							disp_error("Cette case n'est pas vide !");
						}
						if (res == FORBIDDEN) {
							disp_error("Il est interdit de la positionner ici.");
						}
					}
				}


			} else {
				switch (input) {
					case 'N': dir_input = NORTH; break;
					case 'S': dir_input = SOUTH; break;
					case 'E': dir_input = EAST; break;
					case 'O': dir_input = WEST; break;
					case 'G': dir_input = GOAL; break;
				};

				if (is_move_possible(game, dir_input) == 1) {
					move_piece (game, dir_input);
					strncat(history, &input, 1); // add 1 char to history
					strcat(history, " ");
				} else {
					disp_error("Vous ne pouvez pas bouger cette pièce dans cette direction.");
					
				}
			}

			available_movments = movement_left(game);
		} // no movments left anymore


		if (input != 'A') {
			*pcurrent_player = next_player(*pcurrent_player);
		}
		
	}
	
	*pcurrent_player = next_player(*pcurrent_player);
}

int main() {
	int current_player;
	board game = new_game();

	#ifdef DEBUG

	place_piece(game, ONE, SOUTH_P, 0);
	place_piece(game, THREE, SOUTH_P, 1);
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

	current_player = SOUTH_P;

	#else

	init_game(game, &current_player);

	#endif

	clear_screen();
	printf("Fin du placement des pièces, début du jeu !\n");

	gameplay(game, &current_player);

	clear_screen();
	disp_board(game);
	printf("Félicitation joueur %s pour cette victoire", player_name(current_player));
	
	switch (rand()%10) {
		case 0 : printf(" ! Ce fût une belle partie.\n"); break;
		case 1 : printf(" ! C'est mérité.\n"); break;
		case 2 : printf(". J'aurais pas fait ça mais c'est passé, je suppose que c'est bien joué quand même.\n"); break;
		case 3 : printf("\nGG ez.\n"); break;
		case 4 : printf(". Maintenant on joue à un vrai jeu ? Horde ou Alliance ?\n"); break;
		case 5 : printf(" ! Belle connaissance de la méta, solide sur les placements et mental en acier.\n"); break;
		case 6 : printf(". Outplay tout simplement.\n"); break;
		case 7 : printf(". Faut se réveiller joueur %s, c'est votre petit fère qui joue ?\n", player_name(next_player(current_player))); break;
		case 8 : printf(" ! Il y a eu du beau jeu des deux côtés, c'était intéressant.\n"); break;
		case 9 : printf(". Small question to the loser : Do you really speak French ? I have the feeling that you don't understand the rules.\n"); break;
	};

	destroy_game(game);

	return 0;
}