#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "board.h"
#include "bot.h"

// gcc display.c -lSDL2 -lSDL2_image

#define SCREEN_W 750
#define SCREEN_H 600
#define MARGIN_X 100
#define MARGIN_Y 120
#define CELL_SIZE 70

#define DIMENSION 6

typedef struct {
    int line;
    int column;
} position;

typedef struct {
    SDL_Texture *texture;
    SDL_Rect rect;
} image;

bool quit = false;
image controls[6]; // 5 directions + cancel
image placement_first_piece[4];
SDL_Texture * pieces[3]; // 3 sizes

void init_controls(SDL_Renderer *renderer) {
    controls[GOAL].texture = IMG_LoadTexture(renderer, "assets/win.png");
    controls[GOAL].rect.x = 580;
    controls[GOAL].rect.y = 240;

    controls[SOUTH].texture = IMG_LoadTexture(renderer, "assets/arrow_s.png");
    controls[SOUTH].rect.x = 620;
    controls[SOUTH].rect.y = 410;

    controls[NORTH].texture = IMG_LoadTexture(renderer, "assets/arrow_n.png");
    controls[NORTH].rect.x = 620;
    controls[NORTH].rect.y = 310;

    controls[EAST].texture = IMG_LoadTexture(renderer, "assets/arrow_e.png");
    controls[EAST].rect.x = 660;
    controls[EAST].rect.y = 360;

    controls[WEST].texture = IMG_LoadTexture(renderer, "assets/arrow_w.png");
    controls[WEST].rect.x = 570;
    controls[WEST].rect.y = 360;

    controls[5].texture = IMG_LoadTexture(renderer, "assets/cancel.png");
    controls[5].rect.x = 585;
    controls[5].rect.y = 480;

    for (int i = 0; i <= 5; i++) {
        if(!controls[i].texture) fprintf(stderr, "IMG_LoadTexture: %s\n", IMG_GetError());
        SDL_QueryTexture(controls[i].texture, NULL, NULL, &controls[i].rect.w, &controls[i].rect.h); 
    }
}

void init_pieces(SDL_Renderer *renderer) {
    SDL_Surface * surface;
    TTF_Font * font;
    SDL_Color black = {0, 0, 0};
    char txt[2] = "a"; // txt[1] = 0

    font = TTF_OpenFont("assets/joystick.ttf", 55);
    if (!font) {fprintf(stderr, "TTF_OpenFont: %s\n", TTF_GetError());}

    for (int i = 0; i < 3; i++) {
        txt[0] = '1'+i;
        surface = TTF_RenderText_Solid(font, txt, black);
        pieces[i] = SDL_CreateTextureFromSurface(renderer, surface);
    }

    SDL_FreeSurface(surface);
    TTF_CloseFont(font);
}

void init_piecesize(SDL_Renderer *renderer) {
    placement_first_piece[ONE].texture = IMG_LoadTexture(renderer, "assets/1piece.png");
    placement_first_piece[ONE].rect.x = 550;
    placement_first_piece[ONE].rect.y = 0;


    placement_first_piece[TWO].texture = IMG_LoadTexture(renderer, "assets/2piece.png");
    placement_first_piece[TWO].rect.x = 550;
    placement_first_piece[TWO].rect.y = 200;

    placement_first_piece[THREE].texture = IMG_LoadTexture(renderer, "assets/3piece.png");
    placement_first_piece[THREE].rect.x = 550;
    placement_first_piece[THREE].rect.y = 400;

    for (int i = 1; i <= 3; i++) {
        if(!placement_first_piece[i].texture) fprintf(stderr, "IMG_LoadTexture: %s\n", IMG_GetError());
        SDL_QueryTexture(placement_first_piece[i].texture, NULL, NULL, &placement_first_piece[i].rect.w, &placement_first_piece[i].rect.h); 
    }
}

direction direction_clicked(int x, int y) {
    for (direction i = GOAL; i <= WEST+1; i++) {
        if (x > controls[i].rect.x && x < controls[i].rect.x+controls[i].rect.w && y > controls[i].rect.y && y < controls[i].rect.y+controls[i].rect.h) {
            return i;
        }
    }
    return -1;
}

position position_clicked(int x, int y) {
    position response = {-1, -1};

    if (x > MARGIN_X && x < MARGIN_X+DIMENSION*CELL_SIZE && y > MARGIN_Y && y < MARGIN_Y+DIMENSION*CELL_SIZE) {
        response.column = (x-MARGIN_X)/CELL_SIZE;
        response.line = DIMENSION-1 - (y-MARGIN_Y)/CELL_SIZE;
    }
    return response;
}

void clear_screen(SDL_Renderer *renderer) {
    /* background in gray */
    SDL_SetRenderDrawColor(renderer, 160, 160, 160, 255); 
    SDL_RenderClear(renderer);
}

void disp_board(SDL_Renderer *renderer, board game) {
    size piece_size;
    SDL_Rect rect;

    /* draw black lines to make te board */
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    for (int i = 0; i <= DIMENSION; i++) {
        SDL_RenderDrawLine(renderer, MARGIN_X, MARGIN_Y+i*CELL_SIZE, MARGIN_X+DIMENSION*CELL_SIZE, MARGIN_Y+i*CELL_SIZE);
        SDL_RenderDrawLine(renderer, MARGIN_X+i*CELL_SIZE, MARGIN_Y, MARGIN_X+i*CELL_SIZE, MARGIN_Y+DIMENSION*CELL_SIZE);
    }

    for (int line = 0; line < DIMENSION; line++) {
        for (int column = 0; column < DIMENSION; column++) {
            piece_size = get_piece_size(game, line, column);
            if (line == picked_piece_line(game) && column == picked_piece_column(game)) {
                piece_size = picked_piece_size(game);
            }
            if (piece_size > NONE) {
                SDL_QueryTexture(pieces[piece_size-1], NULL, NULL, &rect.w, &rect.h);
                rect.x = MARGIN_X + CELL_SIZE*column + 10;
                rect.y = MARGIN_Y + CELL_SIZE*(DIMENSION-1 - line);
                SDL_RenderCopy(renderer, pieces[piece_size-1], NULL, &rect);

            }
        }
    }
}

size size_clicked(int x, int y) {
    for (size i = ONE; i <= THREE; i++) {
        if (x > placement_first_piece[i].rect.x && x < placement_first_piece[i].rect.x+placement_first_piece[i].rect.w && y > placement_first_piece[i].rect.y && y < placement_first_piece[i].rect.y+placement_first_piece[i].rect.h) {
            return i;
        }
    }
    return -1;
}

void disp_piecesize(SDL_Renderer *renderer, int remaining_piece[]) {
    for (size i = ONE; i <= THREE; i++) {
        if (remaining_piece[i-1] > 0) {
            SDL_RenderCopy(renderer, placement_first_piece[i].texture, NULL, &placement_first_piece[i].rect);
        }
    }
}

void disp_controls(SDL_Renderer *renderer, board game) {
    for (direction dir = GOAL; dir <= WEST; dir++) {
        if (is_move_possible(game, dir)) {
            SDL_RenderCopy(renderer, controls[dir].texture, NULL, &controls[dir].rect);
        }
    }
    if (picked_piece_size(game) != NONE) {
        SDL_RenderCopy(renderer, controls[5].texture, NULL, &controls[5].rect); // cancel
    }
}

void init_sdl(SDL_Window **pscreen, SDL_Renderer **prenderer) {
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {fprintf(stderr, "Erreur d'initialisation de la SDL : %s\n", SDL_GetError());}

    if(IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) fprintf(stderr, "Error: IMG_Init PNG (%s)", IMG_GetError());

    if (TTF_Init() < 0) {fprintf(stderr, "Impossible d'initialiser SDL TTF: %s\n", TTF_GetError());}

    *pscreen = SDL_CreateWindow("Ma fenêtre de jeu", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_W, SCREEN_H, SDL_WINDOW_SHOWN);

    *prenderer = SDL_CreateRenderer(*pscreen, -1, SDL_RENDERER_PRESENTVSYNC);

    if (*pscreen == NULL || *prenderer == NULL) {
        printf("Erreur : %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    init_controls(*prenderer);
    init_pieces(*prenderer);
    init_piecesize(*prenderer);
}

void clean_sdl(SDL_Renderer *renderer) {
    for (direction i = GOAL; i <= WEST+1; i++) {
        SDL_DestroyTexture(controls[i].texture);
    }

    for (int i = 1; i <= 3; i++) {
        SDL_DestroyTexture(pieces[i]);
    }

    SDL_DestroyRenderer(renderer);
    // SDL_DestroyWindow(screen); TO FIX : pass the screen
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}

void disp_message(char *text, TTF_Font *font, SDL_Renderer *renderer) {
    SDL_Surface *surface;
    SDL_Texture *texture;
    SDL_Rect rect;
    SDL_Color black = {0, 0, 0};

    surface = TTF_RenderUTF8_Solid(font, text, black);
    texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
    rect.x = SCREEN_W/2 - rect.w/2;
    rect.y = 50;
    SDL_RenderCopy(renderer, texture, NULL, &rect);
}

char * player_name(player this_player) {
    if (this_player == SOUTH_P) {
        return "SUD";
    }
    if (this_player == NORTH_P) {
        return "NORD";
    }
    return "inconnu";
}

void init_game(SDL_Renderer *renderer, board game, player *pcurrent_player) {
    SDL_Event event;
    return_code response;
    char message[100];
    size clicked = 0;
    int remaining_piece[NB_SIZE];
    position selected;
    selected.line = 0;
    selected.column = 0;
    int compteur = 0;
    TTF_Font *font;
    font = TTF_OpenFont("assets/ubuntu.ttf", 30);
    if (!font) {
        fprintf(stderr, "TTF_OpenFont: %s\n", TTF_GetError());
    }

    if (rand()%2 == 0) {  // random choice of the first player
        *pcurrent_player = NORTH_P;
    } else {
        *pcurrent_player = SOUTH_P;
    }

    SDL_Delay(10);
    clear_screen(renderer);
    disp_board(renderer, game);
    disp_piecesize(renderer, remaining_piece);
    SDL_RenderPresent(renderer);

    for (int i = 0; i < NB_PLAYERS; i++) {
        compteur = 0;
        for(int j = 0; j < NB_SIZE; j++) {
            remaining_piece[j] = NB_INITIAL_PIECES;
        }
        while(compteur < DIMENSION && !quit) {

            clear_screen(renderer);
            sprintf(message, "Joueur %s veuillez selectionner une piece", player_name(*pcurrent_player));
            disp_message(message, font, renderer);
            disp_board(renderer, game);
            disp_piecesize(renderer, remaining_piece);
            SDL_RenderPresent(renderer);

            SDL_WaitEvent(&event);
            while (event.type != SDL_QUIT && (event.type != SDL_MOUSEBUTTONUP || event.button.button != 1)) {
                SDL_WaitEvent(&event);
            }
            if (event.type == SDL_QUIT) {
                quit = true;
            }
            
            
            clicked = size_clicked(event.button.x, event.button.y);

            

            if (clicked == -1 ) {
                clicked = 0;
            }
            if (clicked >= 1) {
                if (remaining_piece[clicked - 1] == 0) {
                    clicked = 0;
                }
            }

            if (clicked >= 1 && !quit) {

                clear_screen(renderer);
                sprintf(message, "La piece %d est selectionné !", clicked);
                disp_message(message, font, renderer);
                disp_board(renderer, game);
                disp_piecesize(renderer, remaining_piece);
                SDL_RenderPresent(renderer);

                SDL_WaitEvent(&event);
                while (event.type != SDL_QUIT && (event.type != SDL_MOUSEBUTTONUP || event.button.button != 1)) {
                    SDL_WaitEvent(&event);
                }

                if (event.type == SDL_QUIT) {
                    quit = true;
                }
                
                selected = position_clicked(event.button.x, event.button.y);

                if ((*pcurrent_player == SOUTH_P && selected.line == southmost_occupied_line(game)) || (*pcurrent_player == NORTH_P && selected.line == northmost_occupied_line(game))) {
                    response = place_piece(game, clicked, *pcurrent_player, selected.column);
                } else {
                    response = PARAM;
                }
                if (response == OK) {
                    remaining_piece[clicked-1]-=1;
                    disp_message(message, font, renderer);
                    disp_board(renderer, game);
                    disp_piecesize(renderer, remaining_piece);
                    SDL_RenderPresent(renderer);
                    compteur++;

                } else if (response == PARAM) {
                    clear_screen(renderer);
                    sprintf(message, "La case n'est pas valide");
                    disp_message(message, font, renderer);
                    disp_board(renderer, game);
                    disp_piecesize(renderer, remaining_piece);
                    SDL_RenderPresent(renderer);
                    SDL_Delay(1000);

                } else if (response == EMPTY) {
                    clear_screen(renderer);
                    sprintf(message, "Il y a déjà une piece ici");
                    disp_message(message, font, renderer);
                    disp_board(renderer, game);
                    disp_piecesize(renderer, remaining_piece);
                    SDL_RenderPresent(renderer);
                    SDL_Delay(1000);

                }
                clicked = 0;
            }
        }
        *pcurrent_player = next_player(*pcurrent_player);    
    }

}

void choose_piece_to_pick(board game, player player) {
    SDL_Event event;
    position clicked;

    while (true) {
        SDL_WaitEvent(&event);
        if (event.type == SDL_QUIT) {
            quit = true;
            return;
        }
        clicked = position_clicked(event.button.x, event.button.y);
        if (pick_piece(game, player, clicked.line, clicked.column) == OK) {
            return;
        }
    }
}

/* return true if the move is canceled */
bool wait_for_move(board game) {
    SDL_Event event;
    direction clicked;

    while (true) {
        SDL_WaitEvent(&event);
        while (event.type != SDL_QUIT && (event.type != SDL_MOUSEBUTTONUP || event.button.button != 1)) {
            SDL_WaitEvent(&event);
        }
        if (event.type == SDL_QUIT) {
            quit = true;
            return false;
        }
        clicked = direction_clicked(event.button.x, event.button.y);
        if (clicked == 5) {
            cancel_step(game);
            return movement_left(game) == -1;
        }
        if (clicked != -1 && move_piece(game, clicked) == OK) {
            return false;
        }
    }
}



int main() {
    SDL_Window *screen;
    SDL_Renderer *renderer;
    TTF_Font *font;
    player current_player;
    board game = new_game();
    char message[100];
    bool move_canceled = false;
    bool firstround = true;
    srand(time(NULL));

    init_sdl(&screen, &renderer);

    font = TTF_OpenFont("assets/ubuntu.ttf", 30);
    if (!font) {fprintf(stderr, "TTF_OpenFont: %s\n", TTF_GetError());}

    // place_piece(game, ONE, SOUTH_P, 0);
    // place_piece(game, THREE, SOUTH_P, 1);
    // place_piece(game, TWO, SOUTH_P, 2);
    // place_piece(game, THREE, SOUTH_P, 3);
    // place_piece(game, ONE, SOUTH_P, 4);
    // place_piece(game, TWO, SOUTH_P, 5);

    // place_piece(game, ONE, NORTH_P, 0);
    // place_piece(game, ONE, NORTH_P, 1);
    // place_piece(game, TWO, NORTH_P, 2);
    // place_piece(game, TWO, NORTH_P, 3);
    // place_piece(game, THREE, NORTH_P, 4);
    // place_piece(game, THREE, NORTH_P, 5);
    
    current_player = NORTH_P;

    while (!quit) {
        clear_screen(renderer);
        if (firstround) {
            init_game(renderer, game, &current_player);
            firstround = false;
            clear_screen(renderer);
        }
        if (movement_left(game) == -1 && !move_canceled && !firstround && !quit) {
            current_player = next_player(current_player);
            sprintf(message, "Joueur %s, à ton tour !", player_name(current_player));
            disp_message(message, font, renderer);
        }

        disp_board(renderer, game);
        disp_controls(renderer, game);
        SDL_RenderPresent(renderer);


        if (movement_left(game) == -1 && !quit) {
            choose_piece_to_pick(game, current_player);
        } else {
            move_canceled = wait_for_move(game);
        }
    }
    

    clean_sdl(renderer);
 
    return EXIT_SUCCESS;
}