#include <stdlib.h>
#include <stdio.h>
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
image commands[6]; // 5 directions + cancel
SDL_Texture * pieces[6]; // 3 sizes * 2 (normal and picked style)

void init_commands(SDL_Renderer *renderer) {
    commands[GOAL].texture = IMG_LoadTexture(renderer, "assets/win.png");
    commands[GOAL].rect.x = 580;
    commands[GOAL].rect.y = 240;

    commands[SOUTH].texture = IMG_LoadTexture(renderer, "assets/arrow_s.png");
    commands[SOUTH].rect.x = 620;
    commands[SOUTH].rect.y = 410;

    commands[NORTH].texture = IMG_LoadTexture(renderer, "assets/arrow_n.png");
    commands[NORTH].rect.x = 620;
    commands[NORTH].rect.y = 310;

    commands[EAST].texture = IMG_LoadTexture(renderer, "assets/arrow_e.png");
    commands[EAST].rect.x = 660;
    commands[EAST].rect.y = 360;

    commands[WEST].texture = IMG_LoadTexture(renderer, "assets/arrow_w.png");
    commands[WEST].rect.x = 570;
    commands[WEST].rect.y = 360;

    commands[5].texture = IMG_LoadTexture(renderer, "assets/cancel.png");
    commands[5].rect.x = 585;
    commands[5].rect.y = 480;

    for (int i = 0; i <= 5; i++) {
        if(!commands[i].texture) fprintf(stderr, "IMG_LoadTexture: %s\n", IMG_GetError());
        SDL_QueryTexture(commands[i].texture, NULL, NULL, &commands[i].rect.w, &commands[i].rect.h); 
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

direction direction_clicked(int x, int y) {
    for (direction i = GOAL; i <= WEST+1; i++) {
        if (x > commands[i].rect.x && x < commands[i].rect.x+commands[i].rect.w && y > commands[i].rect.y && y < commands[i].rect.y+commands[i].rect.h) {
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

void onclick(int x, int y) {
    position p = position_clicked(x, y);
    direction d = direction_clicked(x, y);

    if (p.line != -1) {
        printf("click en case (%d, %d)\n", p.column, p.line);
    } else if (d != -1) {
        printf("click sur commande %d\n", d);
    }
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

void disp_commands(SDL_Renderer *renderer, board game) {
    for (direction dir = GOAL; dir <= WEST; dir++) {
        if (is_move_possible(game, dir)) {
            SDL_RenderCopy(renderer, commands[dir].texture, NULL, &commands[dir].rect);
        }
    }
    if (picked_piece_size(game) != NONE) {
        SDL_RenderCopy(renderer, commands[5].texture, NULL, &commands[5].rect); // cancel
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

    init_commands(*prenderer);
    init_pieces(*prenderer);
}

void clean_sdl(SDL_Renderer *renderer) {
    for (direction i = GOAL; i <= WEST+1; i++) {
        SDL_DestroyTexture(commands[i].texture);
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

void choose_piece_to_pick(board game, player player) {
    SDL_Event event;
    position clicked;

    while (true) {
        SDL_WaitEvent(&event);
        while (event.type != SDL_QUIT && (event.type != SDL_MOUSEBUTTONUP || event.button.button != 1)) {
            SDL_WaitEvent(&event);
        }
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

void wait_for_move(board game) {
    SDL_Event event;
    direction clicked;

    while (true) {
        SDL_WaitEvent(&event);
        while (event.type != SDL_QUIT && (event.type != SDL_MOUSEBUTTONUP || event.button.button != 1)) {
            SDL_WaitEvent(&event);
        }
        if (event.type == SDL_QUIT) {
            quit = true;
            return;
        }
        clicked = direction_clicked(event.button.x, event.button.y);
        if (move_piece(game, clicked) == OK) {
            return;
        }
    }
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

int main() {
    SDL_Window *screen;
    SDL_Renderer *renderer;
    TTF_Font *font;
    player current_player;
    board game = new_game();
    char message[100];

    init_sdl(&screen, &renderer);

    font = TTF_OpenFont("assets/ubuntu.ttf", 30);
    if (!font) {fprintf(stderr, "TTF_OpenFont: %s\n", TTF_GetError());}

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

    current_player = NORTH_P;

    while (!quit) {
        clear_screen(renderer);
        if (movement_left(game) == -1) {
            current_player = next_player(current_player);
            sprintf(message, "Joueur %s, à ton tour !", player_name(current_player));
            disp_message(message, font, renderer);
        }

        disp_board(renderer, game);
        disp_commands(renderer, game);
        SDL_RenderPresent(renderer);

        if (movement_left(game) == -1) {
            choose_piece_to_pick(game, current_player);
        } else {
            wait_for_move(game);
        }
    }
    

    clean_sdl(renderer);
 
    return EXIT_SUCCESS;
}