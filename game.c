#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "board.h"
#include "display.h"

// This macro-function allows us to clear the terminal
#define clear_screen() printf("\033[H\033[2J")

// This one allows us to empty stdin buffer.
#define clear_buffer() while(getchar()!='\n') {}


// Switch lower case tu UPPER case
void capitalize(char * pletter) {
	if (*pletter >= 'a' && *pletter <= 'z') {
		*pletter -= 0x20; // fortunately ASCII table is in a coherent order
	}
}

/* This function is called one time at the begining of the game to:
- Choose a random player to start.
- Ask players to players to place their pieces.

The players have to place all their pieces in the same round because we think it's faster and more ergonomic.
*/
void init_game(board game, player * pcurrent_player) {
	size history[DIMENSION];	// temporarily keep the player's choices
	int column;
	size piece_size;
	return_code response;

	srand(time(NULL));

	if (rand()%2 == 0) {  // random choice of the first player
		*pcurrent_player = NORTH_P;
	} else {
		*pcurrent_player = SOUTH_P;
	}

	for (int i = 0; i < NB_PLAYERS; i++) {	// do the same for all players (i is never used)
		for (int j = 0; j < DIMENSION; j++) { // fill history with NONE size
			history[j] = NONE;
		}

		clear_screen();

		column = 0;
		while (column < DIMENSION) {  // place the pieces column by column
            disp_board(game);
            printf("Joueur %s, veuillez choisir de gauche à droite la taille des pièces à mettre sur votre première ligne.\nValidez avec Entrée pour chaque pièce.\n> ", player_name(*pcurrent_player));

            for (int j = 0; j < DIMENSION; j++) {  // display all the pieces already placed
                if (history[j] != NONE) {
                    printf("%d ", history[j]);
                }
            }

			piece_size = -1;  // we need to do that because if the input is not a number, scanf will not edit the variable
			scanf("%u", &piece_size);
			clear_buffer();
			clear_screen();

			response = place_piece(game, piece_size, *pcurrent_player, column); // != EMPTY because we force the choice of the column
			
			if (response == PARAM) {
				disp_error("Cette taille de pion n'existe pas.");
			}
			if (response == FORBIDDEN) {
				disp_error("Il ne vous reste plus de pion de cette taille-là.");
			}
			if (response == OK) {
				history[column] = piece_size;			
				column++;
			}

			
		} // the first player finished placing every piece

		*pcurrent_player = next_player(*pcurrent_player);
	}
}

/* This function is called instead of init_game when we compile the program with the macro variable DEBUG (make debug).
It enables to set a valid positioning of the pieces to start the main part of the code without having to place 12 pieces manually each time!
*/
void simplified_init_game(board game, player * pcurrent_player) {
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

	*pcurrent_player = SOUTH_P;
}

/* This function is the real main one! It's a bit long but we need to do a lot of things every round:
- Ask for a valid column where we can take a piece,
- Ask for a valid movment (with the possibilty of cancel last steps at every time),
- Enable to choose between "bouncing" or swapping when we arrive on another piece,
- If we swap, ask for a valid position (with the possibility to go back on our choice)
...
*/
void gameplay(board game, player * pcurrent_player) {
	int line;
	int column;
	int nbr_available_movments;
	char * agreement; // contain 's' to print numbers > 1
	char input;
	direction dir_input;
	return_code response;
	int swapping_possible; // 0 or 1
	char history[100];  // a string to be printed
	int input_is_correct; // 0 or 1
	size size_under_picked_piece;
	char next_char;
	
	while (get_winner(game) == NO_PLAYER) {
		history[0] = '\0'; // history = ""

		if (*pcurrent_player == SOUTH_P) {
			line = southmost_occupied_line(game);
		} else {
			line = northmost_occupied_line(game);
		}

		response = -1;		
		while (response != OK) { // ask for a valid column to pick the piece
			disp_board(game);

			printf("Joueur %s, choisissez la colonne où prendre votre pièce (1-6) : ", player_name(*pcurrent_player));
			column = -1; // because scanf doesn't edit the variable if we don't enter a number
			scanf("%d", &column);
			clear_buffer();
			clear_screen();
			column--;

			response = pick_piece(game, *pcurrent_player, line, column); // != FORBIDDEN because we force the choice of the line
			if (response == EMPTY) {
				disp_error("Il n'y a pas de pièce à cet endroit sur la ligne la plus proche de vous.");
			}
			if (response == PARAM) {
				disp_error("Ce numéro est invalide.");
			}
		}

		nbr_available_movments = movement_left(game);
		while (nbr_available_movments != -1) {	
			agreement = plural(nbr_available_movments); // "s" if the number is >1

			input_is_correct = 0;
			while (!input_is_correct) {
				disp_board(game);

				if (nbr_available_movments == 0) {
					size_under_picked_piece = get_piece_size(game, picked_piece_line(game), picked_piece_column(game));
					printf("Vous êtes sur une pièce de taille %d. Vous avez le choix entre :\n", size_under_picked_piece);
					printf("- rebondir de %d case%s : entrez de nouveaux points cardinaux pour vous déplacer\n", size_under_picked_piece, plural(size_under_picked_piece));
					printf("- prendre sa place et la placer ailleurs sur le plateau en faisant P\n> ");
					swapping_possible = 1;

				} else {
					printf("Déplacez-vous en entrant des points cardinaux (N, S, E, O).\nSi vous êtes sur la dernière ligne, faites G pour gagner.\nFaites A pour annuler votre dernier coup.\n\n");
					printf("%d mouvement%s restant%s\n> ", nbr_available_movments, agreement, agreement);
				}

				printf("%s", history);
				scanf("%c", &input);
				capitalize(&input);
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
				response = -1;

				while (response != OK) {
					disp_board(game);
					printf("À tout moment, vous pouvez entrer A pour revenir à l'écran précédent.\n\nChoisissez un nouvel emplacement pour la pièce de taille %d.\n", size_under_picked_piece);
					line = -1;
					printf("Entrez le n° de ligne, 1 étant la ligne Sud (1-6) : ");
					scanf("%d", &line);
					next_char = getchar();
					if (next_char == 'A' && getchar() == '\n') {
						response = OK;
					} else if (next_char != '\n') {
						clear_buffer();
					}
					line--;

					printf("Entrez le n° de colonne (1-6) : ");
					column = -1;
					scanf("%d", &column);
					next_char = getchar();
					if (next_char == 'A' && getchar() == '\n') {
						response = OK;
					} else if (next_char != '\n') {
						clear_buffer();
					}
					column--;

					clear_screen();

					if (response != OK) {
						response = swap_piece(game, line, column);

						if (response == PARAM) {
							disp_error("Vous n'avez pas entré des numéros corrects.");
						}
						if (response == EMPTY) {
							disp_error("Cette case n'est pas vide !");
						}
						if (response == FORBIDDEN) {
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

			nbr_available_movments = movement_left(game);
		} // no movments left anymore


		if (input != 'A') {
			*pcurrent_player = next_player(*pcurrent_player);
		}
		
	}
	
	*pcurrent_player = next_player(*pcurrent_player);
}

int main() {
	player current_player;
	player winner;
	board game = new_game();

	#ifdef DEBUG

	simplified_init_game(game, &current_player);	

	#else

	init_game(game, &current_player);

	#endif

	clear_screen();
	printf("Fin du placement des pièces, début du jeu !\n");

	gameplay(game, &current_player);

	winner = get_winner(game);

	clear_screen();
	disp_board(game);
	printf("Félicitation joueur %s pour cette victoire", player_name(winner));
	
	switch (rand()%10) {
		case 0 : printf(" ! Ce fût une belle partie.\n"); break;
		case 1 : printf(" ! C'est mérité.\n"); break;
		case 2 : printf(". J'aurais pas fait ça mais c'est passé, je suppose que c'est bien joué quand même.\n"); break;
		case 3 : printf("\nGG ez.\n"); break;
		case 4 : printf(". Maintenant on joue à un vrai jeu ? Horde ou Alliance ?\n"); break;
		case 5 : printf(" ! Belle connaissance de la méta, solide sur les placements et mental d'acier.\n"); break;
		case 6 : printf(". Outplay tout simplement.\n"); break;
		case 7 : printf(". Faut se réveiller joueur %s, c'est votre petit fère qui joue ?\n", player_name(next_player(winner))); break;
		case 8 : printf(" ! Il y a eu du beau jeu des deux côtés, c'était intéressant.\n"); break;
		case 9 : printf(". Small question to the loser : Do you really speak French? I have the feeling that you don't understand the rules...\n"); break;
	};

	return 0;
}