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
#define MARGIN_BOTTOM 20
#define DELAY 10

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
    int cell_size;
    board game;
    player current_player;
    bool placement_finished;
    char message[100];
} Env;

void place_controls(Env *env) {
    int window_w, window_h;
    int mid_x, mid_y;

    SDL_GetWindowSize(env->window, &window_w, &window_h);
    mid_x = MARGIN_LEFT + DIMENSION*env->cell_size + (window_w - MARGIN_LEFT - DIMENSION*env->cell_size)/2;
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

void calculate_cell_size(Env * env) {
    int window_w, window_h;
    int cell_size_w, cell_size_h;

    SDL_GetWindowSize(env->window, &window_w, &window_h);

    cell_size_w = (window_w - MARGIN_RIGHT - MARGIN_LEFT)/DIMENSION;
    cell_size_h = (window_h - MARGIN_TOP - MARGIN_BOTTOM)/DIMENSION;

    if (cell_size_w < cell_size_h) {
        env->cell_size = cell_size_w;
    } else {
        env->cell_size = cell_size_h;
    }
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

    if (x > MARGIN_LEFT && x < MARGIN_LEFT+DIMENSION*env->cell_size && y > MARGIN_TOP && y < MARGIN_TOP+DIMENSION*env->cell_size) {
        response.column = (x-MARGIN_LEFT)/env->cell_size;
        response.line = DIMENSION-1 - (y-MARGIN_TOP)/env->cell_size;
    }

    return response;
}

void clear_screen(Env *env) {
    /* background in gray */
    SDL_SetRenderDrawColor(env->renderer, 160, 160, 160, 255); 
    SDL_RenderClear(env->renderer);
}

void disp_board(Env *env) {
    size piece_size;
    SDL_Rect rect;

    /* draw black lines to make te board */
    SDL_SetRenderDrawColor(env->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    for (int i = 0; i <= DIMENSION; i++) {
        SDL_RenderDrawLine(env->renderer, MARGIN_LEFT, MARGIN_TOP+i*env->cell_size, MARGIN_LEFT+DIMENSION*env->cell_size, MARGIN_TOP+i*env->cell_size);
        SDL_RenderDrawLine(env->renderer, MARGIN_LEFT+i*env->cell_size, MARGIN_TOP, MARGIN_LEFT+i*env->cell_size, MARGIN_TOP+DIMENSION*env->cell_size);
    }

    for (int line = 0; line < DIMENSION; line++) {
        for (int column = 0; column < DIMENSION; column++) {
            piece_size = get_piece_size(env->game, line, column);
            if (line == picked_piece_line(env->game) && column == picked_piece_column(env->game)) {
                piece_size = picked_piece_size(env->game);
            }
            if (piece_size > NONE) {
                rect.x = MARGIN_LEFT + env->cell_size*column;
                rect.y = MARGIN_TOP + env->cell_size*(DIMENSION-1 - line);
                rect.w = env->cell_size;
                rect.h = env->cell_size;
                SDL_RenderCopy(env->renderer, env->pieces[piece_size-1], NULL, &rect);

            }
        }
    }
}

void disp_controls(Env *env) {
    for (direction dir = GOAL; dir <= WEST; dir++) {
        if (is_move_possible(env->game, dir)) {
            SDL_RenderCopy(env->renderer, env->controls[dir].texture, NULL, &env->controls[dir].rect);
        }
    }
    if (picked_piece_size(env->game) != NONE) {
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
    calculate_cell_size(env);
}

void clean_sdl(Env *env) {
    for (direction i = GOAL; i <= WEST+1; i++) {
        SDL_DestroyTexture(env->controls[i].texture);
    }

    for (int i = 1; i <= 3; i++) {
        SDL_DestroyTexture(env->pieces[i]);
    }

    SDL_DestroyRenderer(env->renderer);
    SDL_DestroyWindow(env->window);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}

void disp_message(Env *env) {
    SDL_Surface *surface;
    SDL_Texture *texture;
    SDL_Rect rect;
    SDL_Color black = {0, 0, 0};
    int window_w, tmp;

    SDL_GetWindowSize(env->window, &window_w, NULL);

    surface = TTF_RenderUTF8_Solid(env->font, env->message, black);
    texture = SDL_CreateTextureFromSurface(env->renderer, surface);

    SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
    if (rect.w > window_w-15) {
        tmp = rect.w;
        rect.w = window_w-15;
        rect.h = rect.h * rect.w/tmp;
    }
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

bool process_event(Env *env, SDL_Event *event) {
    position pos_clicked;
    direction dir_clicked;

    if (event->type == SDL_QUIT) {
        return true;
    }

    if (event->type == SDL_WINDOWEVENT) {
        calculate_cell_size(env);
        place_controls(env);
        return false;
    }

    if (!env->placement_finished) {
        // TO DO
        return false;
    }

    if (get_winner(env->game) != NO_PLAYER) {
        return false;
    }

    if (event->type == SDL_MOUSEBUTTONUP && event->button.button == 1) {
        if (picked_piece_size(env->game) == NONE) { // choose a piece to pick
            pos_clicked = position_clicked(env, event->button.x, event->button.y);
            if (pick_piece(env->game, env->current_player, pos_clicked.line, pos_clicked.column) == OK) {
                return false;
            }
        } else { // move
            dir_clicked = direction_clicked(env, event->button.x, event->button.y);
            if (dir_clicked == 5) {
                cancel_step(env->game);
                return false;
            }
            if (dir_clicked != -1 && move_piece(env->game, dir_clicked) == OK) {
                if (movement_left(env->game) == -1) {
                    env->current_player = next_player(env->current_player);
                    sprintf(env->message, "Joueur %s, à ton tour !", player_name(env->current_player));
                } else {
                    env->message[0] = 0;
                }
                return false;
            }
        }
    }

    return false;
}

int main() {
    Env env;
    SDL_Event event;
    bool quit = false;

    init_sdl(&env);

    env.game = new_game();
    env.message[0] = 0; // important !

    env.font = TTF_OpenFont("assets/ubuntu.ttf", 30);
    if (!env.font) {fprintf(stderr, "TTF_OpenFont: %s\n", TTF_GetError());}

    random_piece_placement(env.game, SOUTH_P);
    random_piece_placement(env.game, NORTH_P);

    env.placement_finished = true;

    env.current_player = SOUTH_P;
    sprintf(env.message, "Joueur %s, à ton tour !", player_name(env.current_player));

    while (!quit) {
        /* manage events */
        while (SDL_PollEvent(&event)) {
            quit = process_event(&env, &event);
            if(quit) break;
        }

        clear_screen(&env);
        disp_message(&env);
        disp_board(&env);
        disp_controls(&env);
        SDL_RenderPresent(env.renderer);

        SDL_Delay(DELAY);
    }

    clean_sdl(&env);

    return EXIT_SUCCESS;
}