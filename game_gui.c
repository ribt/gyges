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
#define DELAY 1

struct sprite {
    SDL_Texture *texture;
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
    TTF_Font *font;
    enum stage disp_stage;
    SDL_Texture *pieces[3];
    struct sprite controls[6];
    int cell_size;
    board game;
    SDL_Texture *background;
    player current_player;
    player BOT_P;
    move bot_move;
    int dragging_piece;
    struct available_piece initial_pieces[DIMENSION];
    char message[100];
    struct sprite end_buttons[3];
    struct sprite menu_buttons[6];
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

void enable_initial_pieces(Env *env) {
    for (int i = 0; i< DIMENSION; i++) {
        env->initial_pieces[i].size = i/2 + 1;
    }
}

void place_initial_pieces(Env *env) {
    int window_w, window_h;

    SDL_GetWindowSize(env->window, &window_w, &window_h);

    for (int i = 0; i< DIMENSION; i++) {
        env->initial_pieces[i].rect.y = MARGIN_TOP + DIMENSION*env->cell_size + 20;
        env->initial_pieces[i].rect.x = 20 + i*(env->initial_pieces[0].rect.w+10);
        env->initial_pieces[i].rect.w = env->cell_size;
        env->initial_pieces[i].rect.h = env->cell_size;
    }
}


void init_pieces(Env *env) {
    env->pieces[0] = IMG_LoadTexture(env->renderer, "assets/1piece.png");
    env->pieces[1] = IMG_LoadTexture(env->renderer, "assets/2piece.png");
    env->pieces[2] = IMG_LoadTexture(env->renderer, "assets/3piece.png");

    for (int i = 0; i < 3; i++) {
        if(!env->pieces[i]) fprintf(stderr, "IMG_LoadTexture: %s\n", IMG_GetError());
    }

    place_initial_pieces(env);
}

void place_end_buttons(Env *env) {
    int window_w, window_h;

    SDL_GetWindowSize(env->window, &window_w, &window_h);

    env->end_buttons[1].rect.x = window_w/2 - env->end_buttons[1].rect.w/2;
    env->end_buttons[1].rect.y = window_h/2 - env->end_buttons[1].rect.h/2;

    env->end_buttons[0].rect.x = window_w/2 - env->end_buttons[0].rect.w/2;
    env->end_buttons[0].rect.y = env->end_buttons[1].rect.y - 10 - env->end_buttons[0].rect.h;

    env->end_buttons[2].rect.x = window_w/2 - env->end_buttons[2].rect.w/2;
    env->end_buttons[2].rect.y = env->end_buttons[1].rect.y + env->end_buttons[1].rect.h + 10;
}

void place_menu_buttons(Env *env) {
    int window_w, window_h;

    SDL_GetWindowSize(env->window, &window_w, &window_h);

    env->menu_buttons[0].rect.x = window_w/2 - env->menu_buttons[0].rect.w/2;       //TITLE
    env->menu_buttons[0].rect.y = window_h/8;

    env->menu_buttons[1].rect.x = env->menu_buttons[1].rect.w/8;       //PVP
    env->menu_buttons[1].rect.y = window_h/2;

    env->menu_buttons[2].rect.x = env->menu_buttons[1].rect.x;       //PVC
    env->menu_buttons[2].rect.y = window_h/2 + window_h/10;

    env->menu_buttons[3].rect.x = env->menu_buttons[1].rect.x - env->menu_buttons[3].rect.w/8;       //EZ
    env->menu_buttons[3].rect.y = env->menu_buttons[2].rect.y + env->menu_buttons[2].rect.h;

    env->menu_buttons[4].rect.x = env->menu_buttons[3].rect.x + env->menu_buttons[3].rect.w + window_h/10;       //MEDUIM
    env->menu_buttons[4].rect.y = env->menu_buttons[2].rect.y + env->menu_buttons[2].rect.h;

    env->menu_buttons[5].rect.x = env->menu_buttons[4].rect.x + env->menu_buttons[4].rect.w + window_h/10;       //HARD
    env->menu_buttons[5].rect.y = env->menu_buttons[2].rect.y + env->menu_buttons[2].rect.h;

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

void init_menu_buttons(Env *env) {
    TTF_Font *font;
    SDL_Color black = {0, 0, 0};
    SDL_Color red = {165, 0, 0};
    SDL_Color green = {0, 165, 0};

    font = TTF_OpenFont("assets/ubuntu.ttf", 100);

    env->menu_buttons[0].texture = SDL_CreateTextureFromSurface(env->renderer, TTF_RenderUTF8_Solid(font, "Gyges", black));

    font = TTF_OpenFont("assets/ubuntu.ttf", 50);

    env->menu_buttons[1].texture = SDL_CreateTextureFromSurface(env->renderer, TTF_RenderUTF8_Solid(font, "Contre un autre joueur local", black));
    env->menu_buttons[2].texture = SDL_CreateTextureFromSurface(env->renderer, TTF_RenderUTF8_Solid(font, "Contre l'ordinateur :", black));
    env->menu_buttons[3].texture = SDL_CreateTextureFromSurface(env->renderer, TTF_RenderUTF8_Solid(font, "- Facile", green));
    env->menu_buttons[4].texture = SDL_CreateTextureFromSurface(env->renderer, TTF_RenderUTF8_Solid(font, "- Moyen", black));
    env->menu_buttons[5].texture = SDL_CreateTextureFromSurface(env->renderer, TTF_RenderUTF8_Solid(font, "- Difficile", red));

    TTF_CloseFont(font);

    for (int i = 0; i < 6; i++) {
        SDL_QueryTexture(env->menu_buttons[i].texture, NULL, NULL, &env->menu_buttons[i].rect.w, &env->menu_buttons[i].rect.h);
    }

    place_menu_buttons(env);
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

int sprite_clicked(int x, int y, struct sprite sprites[], int len) {
    for (int i = 0; i < len; i++) {
        if (x > sprites[i].rect.x && x < sprites[i].rect.x+sprites[i].rect.w && y > sprites[i].rect.y && y < sprites[i].rect.y+sprites[i].rect.h) {
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

int initial_piece_clicked(Env *env, int x, int y) {
    for (int i = 0; i <= DIMENSION; i++) {
        if (x > env->initial_pieces[i].rect.x && x < env->initial_pieces[i].rect.x+env->initial_pieces[i].rect.w &&
              y > env->initial_pieces[i].rect.y && y < env->initial_pieces[i].rect.y+env->initial_pieces[i].rect.h) {
            return i;
        }
    }
    return -1;
}

void clear_screen(Env *env) {
    /* background in gray */
    SDL_RenderCopy(env->renderer, env->background, NULL, NULL);
    //SDL_SetRenderDrawColor(env->renderer, 160, 160, 160, 255); 
    //SDL_RenderClear(env->renderer);
}

void init_background(Env *env) {
    env->background = IMG_LoadTexture(env->renderer, "assets/background.png");

    if(!env->background) fprintf(stderr, "IMG_LoadTexture: %s\n", IMG_GetError());
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
    for (direction dir = GOAL; dir <= WEST; dir++) {
        if (is_move_possible(env->game, dir)) {
            SDL_RenderCopy(env->renderer, env->controls[dir].texture, NULL, &env->controls[dir].rect);
        }
    }
    if (picked_piece_size(env->game) != NONE) {
        SDL_RenderCopy(env->renderer, env->controls[5].texture, NULL, &env->controls[5].rect); // cancel
    }
}

void disp_sprites(Env *env, struct sprite sprites[], int len) {
    for (int i = 0; i < len; i++) {
        SDL_RenderCopy(env->renderer, sprites[i].texture, NULL, &sprites[i].rect);
    }
}

void start_game(Env *env) {
    env->game = new_game();
    env->disp_stage = PLACEMENT;

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

    env->window = SDL_CreateWindow("Ma fenêtre de jeu", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_W, SCREEN_H, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    env->renderer = SDL_CreateRenderer(env->window, -1, SDL_RENDERER_PRESENTVSYNC);

    if (env->window == NULL || env->renderer == NULL) {
        fprintf(stderr, "Erreur : %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    
    env->message[0] = 0; // message = ""
    env->font = TTF_OpenFont("assets/ubuntu.ttf", 30);
    if (!env->font) {fprintf(stderr, "TTF_OpenFont: %s\n", TTF_GetError());}

    env->dragging_piece = -1;

    env->disp_stage = CONFIG;

    calculate_cell_size(env);
    init_controls(env);
    init_pieces(env);
    init_background(env);
    init_menu_buttons(env);
    init_end_buttons(env);

    return env;
}

void destroy_env(Env *env) {
    destroy_game(env->game);

    for (direction i = GOAL; i <= WEST+1; i++) {
        SDL_DestroyTexture(env->controls[i].texture);
    }

    for (int i = 1; i <= 3; i++) {
        SDL_DestroyTexture(env->pieces[i]);
    }

    for (int i = 0; i < 6; i++) {
        SDL_DestroyTexture(env->menu_buttons[i].texture);
    }

    for (int i = 0; i < 3; i++) {
        SDL_DestroyTexture(env->end_buttons[i].texture);
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
    if (rect.h > MARGIN_TOP) {
        tmp = rect.h;
        rect.h = MARGIN_TOP-10;
        rect.w = rect.w * rect.h/tmp;
    }
    rect.x = window_w/2 - rect.w/2;
    rect.y = MARGIN_TOP/2 - rect.h/2;
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
                        place_initial_pieces(env);
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

void choose_direction(Env *env, SDL_Event *event) {
    int dir_clicked;

    dir_clicked = sprite_clicked(event->button.x, event->button.y, env->controls, 6);
    if (dir_clicked == 5) {
        cancel_step(env->game);
    }
    else if (dir_clicked != -1 && move_piece(env->game, dir_clicked) == OK) {
        if (movement_left(env->game) == -1) {
            if (get_winner(env->game) == NO_PLAYER) {
                env->current_player = next_player(env->current_player);
                sprintf(env->message, "À ton tour %s", player_name(env, env->current_player));
            } else {
                env->disp_stage = END;
                env->current_player = NO_PLAYER;
                sprintf(env->message, "Victoire du %s !", player_name(env, env->current_player));
            }
        } else {
            env->message[0] = 0;
        }
    }
}

void menu_choices(Env *env, SDL_Event *event) {
    int button_clicked = sprite_clicked(event->button.x, event->button.y, env->menu_buttons, 6);

    if (button_clicked <= 0 || button_clicked == 2) {
        return;
    }

    if (button_clicked >= 3 && button_clicked <= 5) {
        env->BOT_P = NORTH_P;
        set_difficulty(button_clicked-3);
    } else {
        env->BOT_P = NO_PLAYER;
    }

    start_game(env);
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
        if (env->disp_stage == CONFIG) {
            place_menu_buttons(env);
        } else if (env->disp_stage == END) {
            place_end_buttons(env);
        } else {
            calculate_cell_size(env);
            if (env->disp_stage == PLACEMENT) {
                place_initial_pieces(env);
            } else if (env->disp_stage == INGAME) {
                place_controls(env);
            }
        }
        return false;
    }


    if (env->disp_stage == CONFIG && event->type == SDL_MOUSEBUTTONUP && event->button.button == 1) {
        menu_choices(env, event);
    }

    if (env->disp_stage == PLACEMENT) {
        drag_initial_pieces(env, event);
    }
  
    if (env->disp_stage == INGAME && event->type == SDL_MOUSEBUTTONUP && event->button.button == 1) {
        if (picked_piece_size(env->game) == NONE) {
            choose_piece_to_pick(env, event);
        } else {
            choose_direction(env, event);
        }
    }

    if (env->disp_stage == END && event->type == SDL_MOUSEBUTTONUP && event->button.button == 1) {
        return end_choices(env, event);
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
            if (get_winner(env->game) == NO_PLAYER) {
                env->current_player = next_player(env->current_player);
                sprintf(env->message, "À ton tour %s", player_name(env, env->current_player));
            } else {
                env->disp_stage = END;
                env->current_player = NO_PLAYER;
                sprintf(env->message, "Victoire du %s !", player_name(env, env->current_player));
            }
        }
    }

    SDL_Delay(500);
}

void render(Env *env) {
    clear_screen(env);
    if (env->disp_stage == CONFIG) {
        disp_sprites(env, env->menu_buttons, 6);
    } else if (env->disp_stage == END) {
        disp_message(env);
        disp_sprites(env, env->end_buttons, 3);
    } else {
        disp_message(env);
        disp_board(env);
        if (env->disp_stage == PLACEMENT) {
            disp_initial_pieces(env);
        } else if (env->disp_stage == INGAME) {
            if (env->current_player != env->BOT_P) {
                disp_controls(env);       
            }
        }
    }
    SDL_RenderPresent(env->renderer);
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

        if (env->current_player == env->BOT_P) {
            play_as_bot(env);
        }
    }

    destroy_env(env);

    return EXIT_SUCCESS;
}