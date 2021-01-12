#include <stdlib.h>
#include <stdio.h>
#include <time.h>
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
#define MARGIN_BOTTOM 100
#define DELAY 10

struct sprite {
    SDL_Texture *texture;
    SDL_Rect rect;
};

struct checkbox_s {
    SDL_Texture *textures[2];
    SDL_Rect rect;
};

struct available_piece {
    size size;
    SDL_Rect rect;    
};

enum stage {CONFIG, PLACEMENT, INGAME, END};

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    int margin_left;
    int margin_top;
    TTF_Font *font;
    enum stage disp_stage;
    SDL_Texture *pieces[3];
    SDL_Texture *picked_pieces[3];
    struct sprite controls[7];
    int cell_size;
    board game;
    SDL_Texture *background;
    player current_player;
    bool swap_allowed;
    int selected_difficuly;
    player BOT_P;
    move bot_move;
    int dragging_piece;
    struct available_piece initial_pieces[DIMENSION];
    char message[100];
    struct sprite menu_buttons[4];
    struct checkbox_s checkbox;
    struct checkbox_s difficulties[4];
    struct sprite end_buttons[3];
} Env;

void place_controls(Env *env) {
    int window_w, window_h;
    int mid_x, mid_y;

    SDL_GetWindowSize(env->window, &window_w, &window_h);
    mid_x = env->margin_left + DIMENSION*env->cell_size + MARGIN_RIGHT/2;
    mid_y = (window_h+env->margin_top/2)/2;

    env->controls[SOUTH].rect.x = mid_x - env->controls[SOUTH].rect.w/2;
    env->controls[SOUTH].rect.y = mid_y + env->controls[EAST].rect.h/2;

    env->controls[NORTH].rect.x = mid_x - env->controls[NORTH].rect.w/2;
    env->controls[NORTH].rect.y = mid_y - env->controls[NORTH].rect.h - env->controls[EAST].rect.h/2;

    env->controls[EAST].rect.x = mid_x + env->controls[NORTH].rect.w/2;
    env->controls[EAST].rect.y = mid_y - env->controls[EAST].rect.h/2;

    env->controls[WEST].rect.x = mid_x - env->controls[NORTH].rect.w/2 - env->controls[EAST].rect.w;
    env->controls[WEST].rect.y = mid_y - env->controls[EAST].rect.h/2;

    env->controls[6].rect.x = mid_x - env->controls[6].rect.w/2;
    env->controls[6].rect.y = env->controls[NORTH].rect.y - 10 - env->controls[6].rect.h;

    env->controls[GOAL].rect.x = mid_x - env->controls[GOAL].rect.w/2;
    env->controls[GOAL].rect.y = env->controls[6].rect.y - 15 - env->controls[GOAL].rect.h;

    env->controls[5].rect.x = mid_x - env->controls[5].rect.w/2;
    env->controls[5].rect.y = env->controls[SOUTH].rect.y + env->controls[SOUTH].rect.h + 15;
}

void init_controls(Env *env) {
    env->controls[GOAL].texture = IMG_LoadTexture(env->renderer, "assets/win.png");
    env->controls[SOUTH].texture = IMG_LoadTexture(env->renderer, "assets/arrow_s.png");
    env->controls[NORTH].texture = IMG_LoadTexture(env->renderer, "assets/arrow_n.png");
    env->controls[EAST].texture = IMG_LoadTexture(env->renderer, "assets/arrow_e.png");   
    env->controls[WEST].texture = IMG_LoadTexture(env->renderer, "assets/arrow_w.png");
    env->controls[5].texture = IMG_LoadTexture(env->renderer, "assets/cancel.png");
    env->controls[6].texture = IMG_LoadTexture(env->renderer, "assets/swap.png");

    for (int i = 0; i <= 6; i++) {
        if(!env->controls[i].texture) fprintf(stderr, "IMG_LoadTexture: %s\n", IMG_GetError());
        SDL_QueryTexture(env->controls[i].texture, NULL, NULL, &env->controls[i].rect.w, &env->controls[i].rect.h);
    }
    place_controls(env);
}

void enable_initial_pieces(Env *env) {
    for (int i = 0; i< DIMENSION; i++) {
        env->initial_pieces[i].size = i/2 + 1;
    }
}

void place_initial_pieces(Env *env) {
    for (int i = 0; i< DIMENSION; i++) {
        env->initial_pieces[i].rect.y = env->margin_top + DIMENSION*env->cell_size + 20;
        env->initial_pieces[i].rect.x = env->margin_left + DIMENSION*env->cell_size/2 + (i - DIMENSION/2)*(env->initial_pieces[0].rect.w+10);
        env->initial_pieces[i].rect.w = env->cell_size;
        env->initial_pieces[i].rect.h = env->cell_size;
    }
}

void init_pieces(Env *env) {
    env->pieces[0] = IMG_LoadTexture(env->renderer, "assets/piece1.png");
    env->pieces[1] = IMG_LoadTexture(env->renderer, "assets/piece2.png");
    env->pieces[2] = IMG_LoadTexture(env->renderer, "assets/piece3.png");

    env->picked_pieces[0] = IMG_LoadTexture(env->renderer, "assets/picked_piece1.png");
    env->picked_pieces[1] = IMG_LoadTexture(env->renderer, "assets/picked_piece2.png");
    env->picked_pieces[2] = IMG_LoadTexture(env->renderer, "assets/picked_piece3.png");

    for (int i = 0; i < 3; i++) {
        if(!env->pieces[i]) fprintf(stderr, "IMG_LoadTexture: %s\n", IMG_GetError());
        if(!env->picked_pieces[i]) fprintf(stderr, "IMG_LoadTexture: %s\n", IMG_GetError());
    }

    place_initial_pieces(env);
}

void place_end_buttons(Env *env) {
    int window_w, window_h;

    SDL_GetWindowSize(env->window, &window_w, &window_h);

    env->end_buttons[1].rect.x = window_w/2 - env->end_buttons[1].rect.w/2;
    env->end_buttons[1].rect.y = window_h/2 - env->end_buttons[1].rect.h/2;

    env->end_buttons[0].rect.x = window_w/2 - env->end_buttons[0].rect.w/2;
    env->end_buttons[0].rect.y = env->end_buttons[1].rect.y - window_h/10 - env->end_buttons[0].rect.h;

    env->end_buttons[2].rect.x = window_w/2 - env->end_buttons[2].rect.w/2;
    env->end_buttons[2].rect.y = env->end_buttons[1].rect.y + env->end_buttons[1].rect.h + window_h/10;
}

void place_menu_sprites(Env *env) {
    int window_w, window_h;

    SDL_GetWindowSize(env->window, &window_w, &window_h);

    env->menu_buttons[0].rect.x = window_w/2 - env->menu_buttons[0].rect.w/2;   // TITLE
    env->menu_buttons[0].rect.y = window_h/10;

    env->menu_buttons[1].rect.x = window_w/8;       // swap autorisé :
    env->menu_buttons[1].rect.y = 2*window_h/5;

    env->checkbox.rect.x = env->menu_buttons[1].rect.x + env->menu_buttons[1].rect.w + 10;
    env->checkbox.rect.y = env->menu_buttons[1].rect.y;
    env->checkbox.rect.h = env->menu_buttons[1].rect.h;
    env->checkbox.rect.w = env->checkbox.rect.h;

    env->menu_buttons[2].rect.x = env->menu_buttons[1].rect.x;       // Niveau de l'ordi
    env->menu_buttons[2].rect.y = env->menu_buttons[1].rect.y + env->menu_buttons[1].rect.h + 20;

    env->difficulties[0].rect.x = window_w/20;
    env->difficulties[0].rect.y = env->menu_buttons[2].rect.y + env->menu_buttons[2].rect.h + 10;

    for (int i = 1; i <= 3; i++) {
        env->difficulties[i].rect.x = env->difficulties[i-1].rect.x + env->difficulties[i-1].rect.w + window_w/20;
        env->difficulties[i].rect.y = env->difficulties[i-1].rect.y;
    }

    env->menu_buttons[3].rect.x = env->difficulties[2].rect.y - window_w/40 - env->menu_buttons[3].rect.w/2;
    env->menu_buttons[3].rect.y = env->difficulties[2].rect.y + env->difficulties[2].rect.h + window_h/15;
}

void init_end_buttons(Env *env) {
    SDL_Color black = {0, 0, 0};

    env->end_buttons[0].texture = SDL_CreateTextureFromSurface(env->renderer, TTF_RenderUTF8_Solid(env->font, "Retour à l'écran d'accueil", black));
    env->end_buttons[1].texture = SDL_CreateTextureFromSurface(env->renderer, TTF_RenderUTF8_Solid(env->font, "Rejouer avec les mêmes paramètres", black));
    env->end_buttons[2].texture = SDL_CreateTextureFromSurface(env->renderer, TTF_RenderUTF8_Solid(env->font, "Quitter le jeu", black));

    for (int i = 0; i < 3; i++) {
        SDL_QueryTexture(env->end_buttons[i].texture, NULL, NULL, &env->end_buttons[i].rect.w, &env->end_buttons[i].rect.h);
    }

    place_end_buttons(env);
}

void init_menu_sprites(Env *env) {
    TTF_Font *font;
    SDL_Color black = {0, 0, 0};

    font = TTF_OpenFont("assets/ubuntu.ttf", 100);

    env->menu_buttons[0].texture = SDL_CreateTextureFromSurface(env->renderer, TTF_RenderUTF8_Solid(font, "Gygès", black));

    TTF_CloseFont(font);
    font = TTF_OpenFont("assets/ubuntu.ttf", 50);

    env->menu_buttons[1].texture = SDL_CreateTextureFromSurface(env->renderer, TTF_RenderUTF8_Solid(font, "Swap autorisé :", black));
    env->menu_buttons[2].texture = SDL_CreateTextureFromSurface(env->renderer, TTF_RenderUTF8_Solid(font, "Niveau de l'ordinateur :", black));

    TTF_CloseFont(font);
    
    env->menu_buttons[3].texture = IMG_LoadTexture(env->renderer, "assets/go.png");

    for (int i = 0; i < 4; i++) {
        SDL_QueryTexture(env->menu_buttons[i].texture, NULL, NULL, &env->menu_buttons[i].rect.w, &env->menu_buttons[i].rect.h);
        if(i >= 3 && !env->menu_buttons[i].texture) fprintf(stderr, "IMG_LoadTexture: %s\n", IMG_GetError());
    }

    env->checkbox.textures[0] = IMG_LoadTexture(env->renderer, "assets/unchecked.png");
    env->checkbox.textures[1] = IMG_LoadTexture(env->renderer, "assets/checked.png");

    env->difficulties[0].textures[0] = IMG_LoadTexture(env->renderer, "assets/aucun.png");
    env->difficulties[0].textures[1] = IMG_LoadTexture(env->renderer, "assets/aucun_selected.png");

    env->difficulties[1].textures[0] = IMG_LoadTexture(env->renderer, "assets/facile.png");
    env->difficulties[1].textures[1] = IMG_LoadTexture(env->renderer, "assets/facile_selected.png");
    
    env->difficulties[2].textures[0] = IMG_LoadTexture(env->renderer, "assets/moyen.png");
    env->difficulties[2].textures[1] = IMG_LoadTexture(env->renderer, "assets/moyen_selected.png");
    
    env->difficulties[3].textures[0] = IMG_LoadTexture(env->renderer, "assets/difficile.png");
    env->difficulties[3].textures[1] = IMG_LoadTexture(env->renderer, "assets/difficile_selected.png");

    for (int i = 0; i < 2; i++) {
        if(!env->checkbox.textures[i]) fprintf(stderr, "IMG_LoadTexture: %s\n", IMG_GetError());
        SDL_QueryTexture(env->checkbox.textures[i], NULL, NULL, &env->checkbox.rect.w, &env->checkbox.rect.h);

        for (int j = 0; j <= 3; j++) {
            if(!env->difficulties[j].textures[i]) fprintf(stderr, "IMG_LoadTexture: %s\n", IMG_GetError());
            SDL_QueryTexture(env->difficulties[j].textures[i], NULL, NULL, &env->difficulties[j].rect.w, &env->difficulties[j].rect.h);
        }
    }

    place_menu_sprites(env);
}

char *player_name(Env *env, player this_player) {
    if (env->BOT_P == NO_PLAYER) {
        if (this_player == SOUTH_P) {
            return "Joueur SUD";
        }
        if (this_player == NORTH_P) {
            return "Joueur NORD";
        }
    } else {
        if (this_player == env->BOT_P) {
            return "Robot";
        } else {
            return "Humain";
        }
    }
    
    return "";
}

bool point_in_rect(int x, int y, SDL_Rect rect) {
    return x > rect.x && x < rect.x+rect.w && y > rect.y && y < rect.y+rect.h;
}

void calculate_cell_size(Env * env) {
    int window_w, window_h;
    int cell_size_w, cell_size_h;
    int tmp;

    SDL_GetWindowSize(env->window, &window_w, &window_h);

    cell_size_w = (window_w - MARGIN_RIGHT - MARGIN_LEFT)/DIMENSION;
    cell_size_h = (window_h - MARGIN_TOP - MARGIN_BOTTOM)/DIMENSION;

    if (cell_size_w < cell_size_h) {
        env->cell_size = cell_size_w;
        tmp = window_h - DIMENSION*env->cell_size;
        if (tmp > MARGIN_TOP*2) {
            env->margin_top = tmp/2;
        } else {
            env->margin_top = MARGIN_TOP;
        }
    } else {
        env->cell_size = cell_size_h;
        tmp = window_w - DIMENSION*env->cell_size;
        if (tmp > MARGIN_RIGHT*2) {
            env->margin_left = tmp/2;
        } else {
            env->margin_left = tmp-MARGIN_RIGHT;
        }
    }
}

int sprite_clicked(int x, int y, struct sprite sprites[], int len) {
    for (int i = 0; i < len; i++) {
        if (point_in_rect(x, y, sprites[i].rect)) {
            return i;
        }
    }
    return -1;
}

position position_clicked(Env *env, int x, int y) {
    position response = {-1, -1};

    if (x > env->margin_left && x < env->margin_left+DIMENSION*env->cell_size && y > env->margin_top && y < env->margin_top+DIMENSION*env->cell_size) {
        response.column = (x-env->margin_left)/env->cell_size;
        response.line = DIMENSION-1 - (y-env->margin_top)/env->cell_size;
    }

    return response;
}

int initial_piece_clicked(Env *env, int x, int y) {
    for (int i = 0; i <= DIMENSION; i++) {
        if (point_in_rect(x, y, env->initial_pieces[i].rect)) {
            return i;
        }
    }
    return -1;
}

void clear_screen(Env *env) {
    SDL_Rect rect;

    /* background in gray */
    SDL_SetRenderDrawColor(env->renderer, 160, 160, 160, 255); 
    SDL_RenderClear(env->renderer);

    if (env->disp_stage != CONFIG) {
        rect.x = env->margin_left - 10;
        rect.y = env->margin_top - 10;
        rect.h = DIMENSION*env->cell_size + 20;
        rect.w = DIMENSION*env->cell_size + 20;
        SDL_RenderCopy(env->renderer, env->background, NULL, &rect);
    }    
}

void init_background(Env *env) {
    env->background = IMG_LoadTexture(env->renderer, "assets/background.png");

    if(!env->background) fprintf(stderr, "IMG_LoadTexture: %s\n", IMG_GetError());
}

void disp_board(Env *env) {
    SDL_Rect rect;
    int mouse_x, mouse_y;

    /* draw black lines to make te board */
    SDL_SetRenderDrawColor(env->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    for (int i = 0; i <= DIMENSION; i++) {
        SDL_RenderDrawLine(env->renderer, env->margin_left, env->margin_top+i*env->cell_size, env->margin_left+DIMENSION*env->cell_size, env->margin_top+i*env->cell_size);
        SDL_RenderDrawLine(env->renderer, env->margin_left+i*env->cell_size, env->margin_top, env->margin_left+i*env->cell_size, env->margin_top+DIMENSION*env->cell_size);
    }

    for (int line = 0; line < DIMENSION; line++) {
        for (int column = 0; column < DIMENSION; column++) {
            rect.x = env->margin_left + env->cell_size*column;
            rect.y = env->margin_top + env->cell_size*(DIMENSION-1 - line);
            rect.w = env->cell_size;
            rect.h = env->cell_size;

            if (line == picked_piece_line(env->game) && column == picked_piece_column(env->game)) {
                SDL_RenderCopy(env->renderer, env->picked_pieces[picked_piece_size(env->game)-1], NULL, &rect);
            }
            else if (get_piece_size(env->game, line, column) > NONE) {
                SDL_RenderCopy(env->renderer, env->pieces[get_piece_size(env->game, line, column)-1], NULL, &rect);
            }
        }
    }

    if (env->dragging_piece > 0 && env->disp_stage == INGAME) {
        SDL_GetMouseState(&mouse_x, &mouse_y);
        rect.x = mouse_x - env->cell_size/2;
        rect.y = mouse_y - env->cell_size/2;
        rect.w = env->cell_size;
        rect.h = env->cell_size;
        SDL_RenderCopy(env->renderer, env->pieces[env->dragging_piece-1], NULL, &rect);
    }
}

void disp_initial_pieces(Env *env) {
    SDL_Rect rect;
    int mouse_x, mouse_y;

    for (int i = 0; i < DIMENSION; i++) {
        if (i == env->dragging_piece) {
            SDL_GetMouseState(&mouse_x, &mouse_y);
            rect.x = mouse_x - env->initial_pieces[i].rect.w/2;
            rect.y = mouse_y - env->initial_pieces[i].rect.h/2;
            rect.w = env->initial_pieces[i].rect.w;
            rect.h = env->initial_pieces[i].rect.h;
            SDL_RenderCopy(env->renderer, env->pieces[env->initial_pieces[i].size-1], NULL, &rect);
        } else if (env->initial_pieces[i].size > NONE) {
            SDL_RenderCopy(env->renderer, env->pieces[env->initial_pieces[i].size-1], NULL, &env->initial_pieces[i].rect);
        }
    }
}

void disp_controls(Env *env) {
    if (picked_piece_size(env->game) != NONE) {
        SDL_RenderCopy(env->renderer, env->controls[5].texture, NULL, &env->controls[5].rect); // cancel
    }

    if (env->dragging_piece == -1) {
        for (direction dir = GOAL; dir <= WEST; dir++) {
            if (is_move_possible(env->game, dir)) {
                SDL_RenderCopy(env->renderer, env->controls[dir].texture, NULL, &env->controls[dir].rect);
            }
        }
        
        if (movement_left(env->game) == 0 && env->swap_allowed) {
            SDL_RenderCopy(env->renderer, env->controls[6].texture, NULL, &env->controls[6].rect); // swap
        }
    }
}

void disp_sprites(Env *env, struct sprite sprites[], int len) {
    for (int i = 0; i < len; i++) {
        if (env->disp_stage == END) {
            SDL_SetRenderDrawColor(env->renderer, 160, 160, 160, 255);
            SDL_RenderFillRect(env->renderer, &sprites[i].rect);
        }
        SDL_RenderCopy(env->renderer, sprites[i].texture, NULL, &sprites[i].rect);
    }
}

void start_game(Env *env) {
    env->game = new_game();
    env->disp_stage = PLACEMENT;

    if (env->selected_difficuly == -1) {
        env->BOT_P = NO_PLAYER;
    } else {
        env->BOT_P = NORTH_P;
        set_difficulty(env->selected_difficuly);
    }

    if (rand()%2 == 0) {  // random choice of the first player
        env->current_player = NORTH_P;
    } else {
        env->current_player = SOUTH_P;
    }

    if (env->current_player != env->BOT_P) {
        enable_initial_pieces(env);
    }

    sprintf(env->message, "%s, place tes pions !", player_name(env, env->current_player));
}

Env *create_env() {
    Env *env = malloc(sizeof(Env));

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {fprintf(stderr, "Erreur d'initialisation de la SDL : %s\n", SDL_GetError());}

    if(IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) fprintf(stderr, "Error: IMG_Init PNG (%s)", IMG_GetError());

    if (TTF_Init() < 0) {fprintf(stderr, "Impossible d'initialiser SDL TTF: %s\n", TTF_GetError());}

    env->window = SDL_CreateWindow("Gygès", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_W, SCREEN_H, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    env->renderer = SDL_CreateRenderer(env->window, -1, SDL_RENDERER_PRESENTVSYNC);

    if (env->window == NULL || env->renderer == NULL) {
        fprintf(stderr, "Erreur : %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    env->margin_left = MARGIN_LEFT;
    env->margin_top = MARGIN_TOP;
    
    env->message[0] = 0; // message = ""
    env->font = TTF_OpenFont("assets/ubuntu.ttf", 30);
    if (!env->font) {fprintf(stderr, "TTF_OpenFont: %s\n", TTF_GetError());}

    env->dragging_piece = -1;
    env->swap_allowed = false;
    env->selected_difficuly = -1;   
    env->BOT_P = NO_PLAYER;

    calculate_cell_size(env);
    init_controls(env);
    init_pieces(env);
    init_background(env);
    init_menu_sprites(env);
    init_end_buttons(env);

    return env;
}

void destroy_env(Env *env) {
    destroy_game(env->game);

    for (int i = 0; i < 7; i++) {
        SDL_DestroyTexture(env->controls[i].texture);
    }

    for (int i = 0; i < 3; i++) {
        SDL_DestroyTexture(env->pieces[i]);
        SDL_DestroyTexture(env->picked_pieces[i]);
    }

    for (int i = 0; i < 4; i++) {
        SDL_DestroyTexture(env->menu_buttons[i].texture);
    }

    for (int i = 0; i < 3; i++) {
        SDL_DestroyTexture(env->end_buttons[i].texture);
    }

    for (int i = 0; i < 2; i++) {
        SDL_DestroyTexture(env->checkbox.textures[i]);
        for (int j = 0; j <= 3; j++) {
            SDL_DestroyTexture(env->difficulties[j].textures[i]);
        }
    }

    SDL_DestroyTexture(env->background);
    TTF_CloseFont(env->font);

    SDL_DestroyRenderer(env->renderer);
    SDL_DestroyWindow(env->window);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
    free(env);
}

void disp_message(Env *env) {
    if (env->message[0] == 0) { // message == ""
        return;
    }
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
    if (rect.h > env->margin_top) {
        tmp = rect.h;
        rect.h = env->margin_top-10;
        rect.w = rect.w * rect.h/tmp;
    }
    rect.x = window_w/2 - rect.w/2;
    rect.y = env->margin_top/2 - rect.h/2;
    SDL_RenderCopy(env->renderer, texture, NULL, &rect);
}

void drag_initial_pieces(Env *env, SDL_Event *event) {
    int piece_clicked;
    position pos_clicked;
    bool done;

    if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == 1) {
        piece_clicked = initial_piece_clicked(env, event->button.x, event->button.y);
        if (piece_clicked > -1) {
            env->dragging_piece = piece_clicked;
        }
    }
    else if (env->dragging_piece > -1 && event->type == SDL_MOUSEBUTTONUP && event->button.button == 1) {
        pos_clicked = position_clicked(env, event->button.x, event->button.y);
        if ((env->current_player == NORTH_P && pos_clicked.line == DIMENSION-1) || (env->current_player == SOUTH_P && pos_clicked.line == 0)) {
            if (place_piece(env->game, env->initial_pieces[env->dragging_piece].size, env->current_player, pos_clicked.column) == OK) {
                env->initial_pieces[env->dragging_piece].size = NONE;

                done = true;
                for (int i = 0; i < DIMENSION; i++) {
                    if (env->initial_pieces[i].size != NONE) {
                        done = false;
                    }
                }
                if (done) {
                    env->current_player = next_player(env->current_player);
                    if (nb_pieces_available(env->game, ONE, env->current_player) == 0) {
                        env->disp_stage = INGAME;
                        sprintf(env->message, "%s, à toi de commencer à jouer !", player_name(env, env->current_player));
                    } else {
                        sprintf(env->message, "%s, place tes pions.", player_name(env, env->current_player));
                        enable_initial_pieces(env);
                    }
                }
            }
        }
        env->dragging_piece = -1;       
    }
}

void choose_piece_to_pick(Env *env, SDL_Event *event) {
    position pos_clicked;

    pos_clicked = position_clicked(env, event->button.x, event->button.y);
    pick_piece(env->game, env->current_player, pos_clicked.line, pos_clicked.column);
}

void end_of_turn(Env *env) {
    player winner;

    winner = get_winner(env->game);

    if (winner == NO_PLAYER) {
        env->current_player = next_player(env->current_player);
        sprintf(env->message, "À ton tour %s", player_name(env, env->current_player));
    } else {
        env->disp_stage = END;
        env->current_player = NO_PLAYER;
        if (env->BOT_P != NO_PLAYER) {
            if (winner == env->BOT_P) {
                sprintf(env->message, "Je t'ai battu, humain !");
            } else {
                sprintf(env->message, "Bien joué humain !");
            }
        } else {
            sprintf(env->message, "Victoire du %s !", player_name(env, get_winner(env->game)));
        }                
    }
}

void choose_direction(Env *env, SDL_Event *event) {
    int dir_clicked;

    dir_clicked = sprite_clicked(event->button.x, event->button.y, env->controls, 7);

    if (dir_clicked == 5) {
        cancel_step(env->game);
    }
    else if (dir_clicked == 6) {
        if (movement_left(env->game) == 0 && env->swap_allowed) {
            env->dragging_piece = get_piece_size(env->game, picked_piece_line(env->game), picked_piece_column(env->game));
            sprintf(env->message, "Pose la pièce sur le plateau.");
            return;
        }
    }
    else if (dir_clicked != -1) {
        move_piece(env->game, dir_clicked);
    }

    if (movement_left(env->game) == 0) {
        sprintf(env->message, "Tu es sur une pièce de taille %d.", get_piece_size(env->game, picked_piece_line(env->game), picked_piece_column(env->game)));
    }
    else if (movement_left(env->game) == -1 && dir_clicked != 5) {
        end_of_turn(env);            
    } else {
        env->message[0] = 0;
    }
}

void drop_dragging_piece(Env *env, SDL_Event *event) {
    position pos_clicked;

    if (sprite_clicked(event->button.x, event->button.y, env->controls, 7) == 5) {
        env->dragging_piece = -1;
        sprintf(env->message, "Tu es sur une pièce de taille %d.", get_piece_size(env->game, picked_piece_line(env->game), picked_piece_column(env->game)));
    }

    else {
        pos_clicked = position_clicked(env, event->button.x, event->button.y);

        if (swap_piece(env->game, pos_clicked.line, pos_clicked.column) == OK) {
            env->dragging_piece = -1;
            end_of_turn(env);
        }
    }
}

void menu_choices(Env *env, SDL_Event *event) {

    if (point_in_rect(event->button.x, event->button.y, env->checkbox.rect)) {
        env->swap_allowed = !env->swap_allowed;
    }

    for (int i = 0; i <= 3; i++) {
        if (point_in_rect(event->button.x, event->button.y, env->difficulties[i].rect)) {    
            env->selected_difficuly = i-1;
        }
    }
    
    if (sprite_clicked(event->button.x, event->button.y, env->menu_buttons, 4) == 3) {
        start_game(env);
    }
}

bool end_choices(Env *env, SDL_Event *event) {
    int button_clicked = sprite_clicked(event->button.x, event->button.y, env->end_buttons, 3);

    if (button_clicked == 0) {
        env->disp_stage = CONFIG;
    } else if (button_clicked == 1) {
        start_game(env);
    } else if (button_clicked == 2) {
        return true;
    }

    return false;
}

bool process_event(Env *env, SDL_Event *event) {  
    if (event->type == SDL_QUIT) {
        return true;
    }

    if (event->type == SDL_WINDOWEVENT) {
        calculate_cell_size(env);
        place_menu_sprites(env);
        place_end_buttons(env);        
        place_initial_pieces(env);
        place_controls(env);

        return false;
    }

    if (env->disp_stage == PLACEMENT) {
        drag_initial_pieces(env, event);
    }

    if (event->type == SDL_MOUSEBUTTONUP && event->button.button == 1) {
        if (env->disp_stage == CONFIG) {
            menu_choices(env, event);
        }

        if (env->disp_stage == INGAME) {
            if (picked_piece_size(env->game) == NONE) {
                choose_piece_to_pick(env, event);
            } else if (env->dragging_piece > 0) {
                drop_dragging_piece(env, event);
            } else {
                choose_direction(env, event);
            }
        }

        if (env->disp_stage == END) {
            return end_choices(env, event);
        }
    } 

    return false;
}

void play_as_bot(Env *env) {
    int i;

    if (env->BOT_P == NO_PLAYER) {
        return;
    }
    if (env->disp_stage == PLACEMENT) {
        random_piece_placement(env->game, env->BOT_P);
        env->current_player = next_player(env->current_player);
        if (nb_pieces_available(env->game, ONE, env->current_player) == 0) {
            env->disp_stage = INGAME;
            sprintf(env->message, "%s, à toi de commencer à jouer !", player_name(env, env->current_player));
        } else {
            place_initial_pieces(env);
            sprintf(env->message, "%s, place tes pions.", player_name(env, env->current_player));
            enable_initial_pieces(env);
        }
        return;
    }
    if (picked_piece_size(env->game) == NONE) {
        env->bot_move = bot_move(env->game, env->BOT_P);
        pick_piece(env->game, env->BOT_P, env->bot_move.piece.line, env->bot_move.piece.column);
    } else {
        i = 0;
        while (i < env->bot_move.path.len && env->bot_move.path.directions[i] == -1) {
            i++;
        }
        move_piece(env->game, env->bot_move.path.directions[i]);
        env->bot_move.path.directions[i] = -1;
        if (movement_left(env->game) == -1) {
            end_of_turn(env);
        }
    }

    SDL_Delay(500);
}

void disp_menu(Env *env) {
    disp_sprites(env, env->menu_buttons, 4);

    SDL_RenderCopy(env->renderer, env->checkbox.textures[env->swap_allowed], NULL, &env->checkbox.rect);

    for (int i = 0; i <= 3; i++) {
        if (env->selected_difficuly == i-1) {
            SDL_RenderCopy(env->renderer, env->difficulties[i].textures[1], NULL, &env->difficulties[i].rect);
        } else {
            SDL_RenderCopy(env->renderer, env->difficulties[i].textures[0], NULL, &env->difficulties[i].rect);
        }
    }
}

void render(Env *env) {
    clear_screen(env);
    if (env->disp_stage == CONFIG) {
        disp_menu(env);
    } else {
        disp_message(env);
        disp_board(env);
        if (env->disp_stage == PLACEMENT) {
            disp_initial_pieces(env);
        } else if (env->disp_stage == INGAME) {
            if (env->current_player != env->BOT_P) {
                disp_controls(env);       
            }
        } else if (env->disp_stage == END) {
            disp_sprites(env, env->end_buttons, 3);
        }
    }
}

void pause() {
    SDL_Event event;
    SDL_WaitEvent(&event);
    while (event.type != SDL_QUIT) {
        SDL_WaitEvent(&event);
    }
}

int main() {
    Env *env;
    SDL_Event event;
    bool quit = false;

    srand(time(NULL));

    env = create_env();

    while (!quit) {
        /* manage events */
        while (SDL_PollEvent(&event)) {
            quit = process_event(env, &event);
            if(quit) break;
        }

        render(env);

        SDL_RenderPresent(env->renderer);
        SDL_Delay(DELAY);

        if (env->current_player == env->BOT_P) {
            play_as_bot(env);
        }
    }

    destroy_env(env);

    return EXIT_SUCCESS;
}