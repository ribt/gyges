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

image commands[6]; // 5 directions + cancel
SDL_Texture * pieces[3];

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
        if(!commands[i].texture) fprintf(stderr, "IMG_LoadTexture: %s\n", SDL_GetError());
        SDL_QueryTexture(commands[i].texture, NULL, NULL, &commands[i].rect.w, &commands[i].rect.h); 
    }
}

void init_pieces(SDL_Renderer *renderer) {
    SDL_Surface * surface;
    TTF_Font * font;
    SDL_Color black = {0, 0, 0};
    char txt[2] = "1";

    font = TTF_OpenFont("assets/joystick.ttf", 55);
    if (!font) {fprintf(stderr, "TTF_OpenFont: %s\n", TTF_GetError());}

    for (int i = 1; i <= 3; i++) {
        txt[0] = '0'+i;
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
        response.line = (y-MARGIN_Y)/CELL_SIZE;
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

void display_flip(SDL_Renderer *renderer, board game) {
    size piece_size;
    SDL_Rect rect;

    /* background in gray */
    SDL_SetRenderDrawColor(renderer, 160, 160, 160, 255); 
    SDL_RenderClear(renderer);

    /* draw black lines to make te board */
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    for (int i = 0; i <= DIMENSION; i++) {
        SDL_RenderDrawLine(renderer, MARGIN_X, MARGIN_Y+i*CELL_SIZE, MARGIN_X+DIMENSION*CELL_SIZE, MARGIN_Y+i*CELL_SIZE);
        SDL_RenderDrawLine(renderer, MARGIN_X+i*CELL_SIZE, MARGIN_Y, MARGIN_X+i*CELL_SIZE, MARGIN_Y+DIMENSION*CELL_SIZE);
    }

    /* display the commands */
    for (direction i = GOAL; i <= WEST+1; i++) {
        SDL_RenderCopy(renderer, commands[i].texture, NULL, &commands[i].rect);
    }

    for (int line = 0; line < DIMENSION; line++) {
        for (int column = 0; column < DIMENSION; column++) {
            piece_size = get_piece_size(game, line, column);
            if (piece_size > 0) {
                SDL_QueryTexture(pieces[piece_size], NULL, NULL, &rect.w, &rect.h);
                rect.x = MARGIN_X + CELL_SIZE*column + 10;
                rect.y = MARGIN_Y + CELL_SIZE*line;
                SDL_RenderCopy(renderer, pieces[piece_size], NULL, &rect);

            }
        }
    }

    SDL_RenderPresent(renderer);
    SDL_Delay(1);
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

void clean_sdl(SDL_Window *screen, SDL_Renderer *renderer) {
    for (direction i = GOAL; i <= WEST+1; i++) {
        SDL_DestroyTexture(commands[i].texture);
    }

    for (int i = 1; i <= 3; i++) {
        SDL_DestroyTexture(pieces[i]);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(screen);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}

int main() {
    SDL_Window *screen;
    SDL_Renderer *renderer;
    SDL_Event event;
    bool quit = false;
    // player current_player;
    board game = new_game();

    init_sdl(&screen, &renderer);

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

    // current_player = SOUTH_P;
    
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_MOUSEBUTTONUP:
                    if (event.button.button == 1) {
                        onclick(event.button.x, event.button.y);
                    }
                    break;
            }
            if (quit) break;
        }

       display_flip(renderer, game);
    }

    clean_sdl(screen, renderer);
 
    return EXIT_SUCCESS;
}