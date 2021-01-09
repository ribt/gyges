#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "board.h"
#include "bot.h"

// gcc game_gui.c -lSDL2 -lSDL2_image -lSDL2_ttf

#define SCREEN_W 750
#define SCREEN_H 600
#define MARGIN_LEFT 100
#define MARGIN_RIGHT 230
#define MARGIN_TOP 120

typedef struct {
    int line;
    int column;
} position;

typedef struct {
    SDL_Texture *texture;
    SDL_Rect rect;
} image;

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    TTF_Font *font;
    SDL_Texture *pieces[3];
    image controls[6];
} Env;


bool quit = false;

void place_controls(Env *env) {
    int window_w, window_h;
    int mid_x, mid_y;

    SDL_GetWindowSize(env->window, &window_w, &window_h);
    mid_x = window_w - 0.5*MARGIN_RIGHT;
    mid_y = (window_h+MARGIN_TOP/2)/2;

    env->controls[SOUTH].rect.x = mid_x - 0.5*env->controls[SOUTH].rect.w;
    env->controls[SOUTH].rect.y = mid_y + 0.5*env->controls[EAST].rect.h;

    env->controls[NORTH].rect.x = mid_x - 0.5*env->controls[NORTH].rect.w;
    env->controls[NORTH].rect.y = mid_y - env->controls[NORTH].rect.h - 0.5*env->controls[EAST].rect.h;

    env->controls[EAST].rect.x = mid_x + 0.5*env->controls[NORTH].rect.w;
    env->controls[EAST].rect.y = mid_y - 0.5*env->controls[EAST].rect.h;

    env->controls[WEST].rect.x = mid_x - 0.5*env->controls[NORTH].rect.w - env->controls[EAST].rect.w;
    env->controls[WEST].rect.y = mid_y - 0.5*env->controls[EAST].rect.h;

    env->controls[GOAL].rect.x = mid_x - 0.5*env->controls[GOAL].rect.w;
    env->controls[GOAL].rect.y = env->controls[NORTH].rect.y - 15 - env->controls[GOAL].rect.h;

    env->controls[5].rect.x = mid_x - 0.5*env->controls[5].rect.w;
    env->controls[5].rect.y = env->controls[SOUTH].rect.y + env->controls[SOUTH].rect.h + 15;
}

void init_controls(Env *env) {
    env->controls[SOUTH].texture = IMG_LoadTexture(env->renderer, "assets/arrow_s.png");
    env->controls[NORTH].texture = IMG_LoadTexture(env->renderer, "assets/arrow_n.png");
    env->controls[EAST].texture = IMG_LoadTexture(env->renderer, "assets/arrow_e.png");   
    env->controls[WEST].texture = IMG_LoadTexture(env->renderer, "assets/arrow_w.png");
    env->controls[GOAL].texture = IMG_LoadTexture(env->renderer, "assets/win.png");
    env->controls[5].texture = IMG_LoadTexture(env->renderer, "assets/cancel.png");

    for (int i = 0; i <= 5; i++) {
        if(!env->controls[i].texture) fprintf(stderr, "IMG_LoadTexture: %s\n", IMG_GetError());
        SDL_QueryTexture(env->controls[i].texture, NULL, NULL, &env->controls[i].rect.w, &env->controls[i].rect.h);
    }
    place_controls(env);
}

void init_pieces(Env *env) {
    SDL_Surface * surface;
    TTF_Font * font;
    SDL_Color black = {0, 0, 0};
    char txt[2] = "a"; // txt[1] = 0

    font = TTF_OpenFont("assets/joystick.ttf", 55);
    if (!font) {fprintf(stderr, "TTF_OpenFont: %s\n", TTF_GetError());}

    for (int i = 0; i < 3; i++) {
        txt[0] = '1'+i;
        surface = TTF_RenderText_Solid(font, txt, black);
        env->pieces[i] = SDL_CreateTextureFromSurface(env->renderer, surface);
    }

    SDL_FreeSurface(surface);
    TTF_CloseFont(font);
}

direction direction_clicked(Env *env, int x, int y) {
    for (direction i = GOAL; i <= WEST+1; i++) {
        if (x > env->controls[i].rect.x && x < env->controls[i].rect.x+env->controls[i].rect.w && y > env->controls[i].rect.y && y < env->controls[i].rect.y+env->controls[i].rect.h) {
            return i;
        }
    }
    return -1;
}

position position_clicked(Env *env, int x, int y) {
    position response = {-1, -1};
    int window_w;
    int cell_size;

    SDL_GetWindowSize(env->window, &window_w, NULL);
    cell_size = (window_w - MARGIN_RIGHT - MARGIN_LEFT)/DIMENSION;

    if (x > MARGIN_LEFT && x < MARGIN_LEFT+DIMENSION*cell_size && y > MARGIN_TOP && y < MARGIN_TOP+DIMENSION*cell_size) {
        response.column = (x-MARGIN_LEFT)/cell_size;
        response.line = DIMENSION-1 - (y-MARGIN_TOP)/cell_size;
    }

    return response;
}

void clear_screen(Env *env) {
    /* background in gray */
    SDL_SetRenderDrawColor(env->renderer, 160, 160, 160, 255); 
    SDL_RenderClear(env->renderer);
}

void disp_board(Env *env, board game) {
    size piece_size;
    SDL_Rect rect;
    int window_w;
    int cell_size;

    SDL_GetWindowSize(env->window, &window_w, NULL);
    cell_size = (window_w - MARGIN_RIGHT -MARGIN_LEFT)/DIMENSION;

    /* draw black lines to make te board */
    SDL_SetRenderDrawColor(env->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    for (int i = 0; i <= DIMENSION; i++) {
        SDL_RenderDrawLine(env->renderer, MARGIN_LEFT, MARGIN_TOP+i*cell_size, MARGIN_LEFT+DIMENSION*cell_size, MARGIN_TOP+i*cell_size);
        SDL_RenderDrawLine(env->renderer, MARGIN_LEFT+i*cell_size, MARGIN_TOP, MARGIN_LEFT+i*cell_size, MARGIN_TOP+DIMENSION*cell_size);
    }

    for (int line = 0; line < DIMENSION; line++) {
        for (int column = 0; column < DIMENSION; column++) {
            piece_size = get_piece_size(game, line, column);
            if (line == picked_piece_line(game) && column == picked_piece_column(game)) {
                piece_size = picked_piece_size(game);
            }
            if (piece_size > NONE) {
                SDL_QueryTexture(env->pieces[piece_size-1], NULL, NULL, &rect.w, &rect.h);
                rect.x = MARGIN_LEFT + cell_size*column + 10;
                rect.y = MARGIN_TOP + cell_size*(DIMENSION-1 - line);
                SDL_RenderCopy(env->renderer, env->pieces[piece_size-1], NULL, &rect);

            }
        }
    }
}

void disp_controls(Env *env, board game) {
    for (direction dir = GOAL; dir <= WEST; dir++) {
        if (is_move_possible(game, dir)) {
            SDL_RenderCopy(env->renderer, env->controls[dir].texture, NULL, &env->controls[dir].rect);
        }
    }
    if (picked_piece_size(game) != NONE) {
        SDL_RenderCopy(env->renderer, env->controls[5].texture, NULL, &env->controls[5].rect); // cancel
    }
}

void init_sdl(Env *env) {
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {fprintf(stderr, "Erreur d'initialisation de la SDL : %s\n", SDL_GetError());}

    if(IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) fprintf(stderr, "Error: IMG_Init PNG (%s)", IMG_GetError());

    if (TTF_Init() < 0) {fprintf(stderr, "Impossible d'initialiser SDL TTF: %s\n", TTF_GetError());}

    env->window = SDL_CreateWindow("Ma fenêtre de jeu", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_W, SCREEN_H, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    env->renderer = SDL_CreateRenderer(env->window, -1, SDL_RENDERER_PRESENTVSYNC);

    if (env->window == NULL || env->renderer == NULL) {
        printf("Erreur : %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    init_controls(env);
    init_pieces(env);
}

void clean_sdl(Env *env) {
    for (direction i = GOAL; i <= WEST+1; i++) {
        SDL_DestroyTexture(env->controls[i].texture);
    }

    for (int i = 1; i <= 3; i++) {
        SDL_DestroyTexture(env->pieces[i]);
    }

    SDL_DestroyRenderer(env->renderer);
    // SDL_DestroyWindow(screen); TO FIX : pass the screen
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}

void choose_piece_to_pick(Env *env, board game, player player) {
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
        clicked = position_clicked(env, event.button.x, event.button.y);
        if (pick_piece(game, player, clicked.line, clicked.column) == OK) {
            return;
        }
    }
}

/* return true if the move is canceled */
bool wait_for_move(Env *env, board game) {
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
        clicked = direction_clicked(env, event.button.x, event.button.y);
        if (clicked == 5) {
            cancel_step(game);
            return movement_left(game) == -1;
        }
        if (clicked != -1 && move_piece(game, clicked) == OK) {
            return false;
        }
    }
}

void disp_message(Env *env, char *text) {
    SDL_Surface *surface;
    SDL_Texture *texture;
    SDL_Rect rect;
    SDL_Color black = {0, 0, 0};
    int window_w;

    SDL_GetWindowSize(env->window, &window_w, NULL);

    surface = TTF_RenderUTF8_Solid(env->font, text, black);
    texture = SDL_CreateTextureFromSurface(env->renderer, surface);

    SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
    rect.x = window_w/2 - rect.w/2;
    rect.y = 50;
    SDL_RenderCopy(env->renderer, texture, NULL, &rect);
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
    Env env;
    player current_player;
    board game = new_game();
    char message[100];
    bool move_canceled = false;

    init_sdl(&env);

    env.font = TTF_OpenFont("assets/ubuntu.ttf", 30);
    if (!env.font) {fprintf(stderr, "TTF_OpenFont: %s\n", TTF_GetError());}

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
        clear_screen(&env);
        if (movement_left(game) == -1 && !move_canceled) {
            current_player = next_player(current_player);
            sprintf(message, "Joueur %s, à ton tour !", player_name(current_player));
            disp_message(&env, message);
        }

        disp_board(&env, game);
        disp_controls(&env, game);
        SDL_RenderPresent(env.renderer);

        if (movement_left(game) == -1) {
            choose_piece_to_pick(&env, game, current_player);
        } else {
            move_canceled = wait_for_move(&env, game);
        }
    }
    

    clean_sdl(&env);
 
    return EXIT_SUCCESS;
}