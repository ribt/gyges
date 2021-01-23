#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "board.h"
#include "bot.h"
#include "socket.h"

#define SCREEN_W 750
#define SCREEN_H 600
#define MARGIN_LEFT 100
#define MARGIN_RIGHT 230
#define MARGIN_TOP 120
#define MARGIN_BOTTOM 100
#define DELAY 10
#define PORT 2002

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

typedef struct {
    size size;
    position position;
} piece;


// display stages
enum stage {TITLE, SOLO, MULTI, PLACEMENT, INGAME, END};

// "global" variables used by all the functions
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
    int selected_difficulty;
    player BOT_P;
    player SOCKET_P;
    move bot_move;
    int dragging_piece;
    struct available_piece initial_pieces[DIMENSION];
    char message[100];
    struct sprite title_sprites[5];
    struct sprite solo_sprites[1];
    struct sprite multi_sprites[3];
    struct checkbox_s checkbox;
    struct sprite difficulties[3];
    struct sprite end_buttons[3];
    struct server_sockets serv_sockets;
    SOCKET socket;
} Env;


// Utility functions :

char *player_name(Env *env, player this_player);
bool point_in_rect(int x, int y, SDL_Rect rect);
void disp_sprites(Env *env, struct sprite sprites[], int len);
void end_of_turn(Env *env);
void play_as_bot(Env *env);
void remote_plays(Env *env);


int initial_piece_clicked(Env *env, int x, int y);
int sprite_clicked(int x, int y, struct sprite sprites[], int len);
position position_clicked(Env *env, int x, int y);


// Init functions (load images, fonts...):

Env *create_env();
void start_game(Env *env);

void init_title_sprites(Env *env);
void init_solo_sprites(Env *env);
void init_multi_sprites(Env *env);
void init_pieces(Env *env);
void enable_initial_pieces(Env *env);
void init_background(Env *env);
void init_controls(Env *env);
void init_end_buttons(Env *env);


// Functions used to render the display:

void render(Env *env);

void clear_screen(Env *env);
void disp_titlescreen(Env *env);
void disp_message(Env *env);
void disp_initial_pieces(Env *env);
void disp_board(Env *env);
void disp_controls(Env *env);
void disp_winner_piece(Env *env);


// Functions used to process events:

bool process_event(Env *env, SDL_Event *event);

void menu_choices(Env *env, SDL_Event *event);
void drag_initial_pieces(Env *env, SDL_Event *event);
void choose_piece_to_pick(Env *env, SDL_Event *event);
void choose_direction(Env *env, SDL_Event *event);
void drop_dragging_piece(Env *env, SDL_Event *event);
bool end_choices(Env *env, SDL_Event *event);


// Functions to adapt the display when the window is resized:

void place_title_sprites(Env *env);
void place_solo_sprites(Env *env);
void place_multi_sprites(Env *env);
void calculate_cell_size(Env * env);
void place_initial_pieces(Env *env);
void place_controls(Env *env);
void place_end_buttons(Env *env);


// Function to clean the memory:

void destroy_env(Env *env);


int main(int argc, char *argv[]) {
    Env *env;
    SDL_Event event;
    bool quit = false;

    srand(time(NULL));

    env = create_env();

    while (!quit) {
        while (SDL_PollEvent(&event)) {
            quit = process_event(env, &event);
            if (quit) {
                break;
            }
        }

        render(env);

        SDL_RenderPresent(env->renderer);
        SDL_Delay(DELAY);

        if (env->current_player == env->BOT_P) {
            play_as_bot(env);
        } else if (env->current_player == env->SOCKET_P) {
            remote_plays(env);
        }
    }

    destroy_env(env);

    return EXIT_SUCCESS;
}

Env *create_env() {
    Env *env = malloc(sizeof(Env));

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {fprintf(stderr, "Erreur d'initialisation de la SDL : %s\n", SDL_GetError());}

    if(IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {fprintf(stderr, "Impossible d'initialiser SDL Image PNG : %s\n", IMG_GetError());}

    if(TTF_Init() < 0) {fprintf(stderr, "Impossible d'initialiser SDL TTF : %s\n", TTF_GetError());}

    env->window = SDL_CreateWindow("Gygès", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_W, SCREEN_H, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    env->renderer = SDL_CreateRenderer(env->window, -1, SDL_RENDERER_PRESENTVSYNC);

    if (env->window == NULL || env->renderer == NULL) {
        fprintf(stderr, "Erreur : %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_SetRenderDrawBlendMode(env->renderer, SDL_BLENDMODE_BLEND);

    env->margin_left = MARGIN_LEFT;
    env->margin_top = MARGIN_TOP;
    
    env->message[0] = 0; // message = ""
    env->font = TTF_OpenFont("assets/ubuntu.ttf", 30);
    if (!env->font) {fprintf(stderr, "TTF_OpenFont: %s\n", TTF_GetError());}

    env->dragging_piece = -1;
    env->swap_allowed = false;
    env->selected_difficulty = 2;   
    env->BOT_P = NO_PLAYER;

    calculate_cell_size(env);
    init_title_sprites(env);
    init_solo_sprites(env);
    init_multi_sprites(env);
    init_background(env);
    init_pieces(env);
    init_controls(env); 
    init_end_buttons(env);

    return env;
}

void render(Env *env) {
    clear_screen(env);

    if (env->disp_stage < PLACEMENT) {
        disp_titlescreen(env);
    } else {
        disp_board(env);
        if (env->disp_stage == PLACEMENT) {
            disp_initial_pieces(env);
        } else if (env->disp_stage == INGAME) {
            if (env->current_player != env->BOT_P && env->current_player != env->SOCKET_P) {
                disp_controls(env);       
            }
        } else if (env->disp_stage == END) {
            disp_winner_piece(env);
            disp_sprites(env, env->end_buttons, 3);
        }
        disp_message(env);
    }
}

bool process_event(Env *env, SDL_Event *event) {  
    if (event->type == SDL_QUIT) {
        return true;
    }

    if (event->type == SDL_WINDOWEVENT) {
        calculate_cell_size(env);
        place_title_sprites(env);
        place_initial_pieces(env);
        place_controls(env);
        place_end_buttons(env);

        return false;
    }

    if (env->disp_stage == PLACEMENT) {
        drag_initial_pieces(env, event);
    }

    if (event->type == SDL_MOUSEBUTTONUP && event->button.button == 1) { // left click
        if (env->disp_stage < PLACEMENT) {
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
            sprintf(env->message, "%s, place tes pions.", player_name(env, env->current_player));
            enable_initial_pieces(env);
        }
    }
    else if (env->disp_stage == INGAME) {
        if (picked_piece_size(env->game) == NONE) { // chose what to play
            env->bot_move = bot_move(env->game, env->BOT_P);
            pick_piece(env->game, env->BOT_P, env->bot_move.piece.line, env->bot_move.piece.column);
        } else { // display previous choice
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
    }

    SDL_Delay(750);
}

void remote_plays(Env *env) {
    piece r_piece;
    direction r_direction;
    bool done;

    if (env->SOCKET_P == NO_PLAYER) {
        return;
    }
    if (env->disp_stage == PLACEMENT) {
        printf("\nattente d'une pièce pour placer\n");
        if (recv(env->socket, &r_piece, sizeof(piece), 0) != SOCKET_ERROR) {
            printf("reçu : size=%d line=%d column=%d\n", r_piece.size, r_piece.position.line, r_piece.position.column);
            place_piece(env->game, r_piece.size, env->SOCKET_P, r_piece.position.column);
            done = true;
            for (size s = ONE; s <= THREE; s++) {
                if (nb_pieces_available(env->game, s, env->SOCKET_P) > 0) {
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
        } else {printf("erreur\n");}
    }
    else if (env->disp_stage == INGAME) {
        if (picked_piece_size(env->game) == NONE) {
            printf("\nattente d'une pièce pour picker\n");
            if (recv(env->socket, &r_piece, sizeof(piece), 0) != SOCKET_ERROR) {
                printf("reçu : size=%d line=%d column=%d\n", r_piece.size, r_piece.position.line, r_piece.position.column);
                pick_piece(env->game, env->SOCKET_P, r_piece.position.line, r_piece.position.column);
            } else {printf("erreur\n");}
        } else {
            printf("\nattente d'une direction\n");
            if (recv(env->socket, &r_direction, sizeof(direction), 0) != SOCKET_ERROR) {
                printf("reçu : %d\n", r_direction);
                move_piece(env->game, r_direction);
                if (movement_left(env->game) == -1) {
                    end_of_turn(env);
                }
            } else {printf("erreur\n");}
        }
    }
}

void destroy_env(Env *env) {
    destroy_game(env->game);

    SDL_DestroyTexture(env->background);
    TTF_CloseFont(env->font);

    for (int i = 0; i <= 4; i++) {
        SDL_DestroyTexture(env->title_sprites[i].texture);
    }

    SDL_DestroyTexture(env->solo_sprites[0].texture);

    for (int i = 0; i <= 2; i++) {
        SDL_DestroyTexture(env->multi_sprites[i].texture);
    }

    for (int i = 0; i < 2; i++) {
        SDL_DestroyTexture(env->checkbox.textures[i]);
    }

    for (int i = 0; i < 3; i++) {
        SDL_DestroyTexture(env->difficulties[i].texture);
    }

    for (int i = 0; i < 3; i++) {
        SDL_DestroyTexture(env->pieces[i]);
        SDL_DestroyTexture(env->picked_pieces[i]);
    }

    for (int i = 0; i < 7; i++) {
        SDL_DestroyTexture(env->controls[i].texture);
    }

    for (int i = 0; i < 3; i++) {
        SDL_DestroyTexture(env->end_buttons[i].texture);
    }

    SDL_DestroyRenderer(env->renderer);
    SDL_DestroyWindow(env->window);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
    free(env);
}

char *player_name(Env *env, player this_player) {
    if (env->BOT_P == NO_PLAYER && env->SOCKET_P == NO_PLAYER) {
        if (this_player == SOUTH_P) {
            return "Joueur SUD";
        }
        if (this_player == NORTH_P) {
            return "Joueur NORD";
        }
    } else if (env->BOT_P != NO_PLAYER) {
        if (this_player == env->BOT_P) {
            return "Robot";
        } else {
            return "Humain";
        }
    } else {
        if (this_player == env->SOCKET_P) {
            return "Distant";
        } else {
            return "Local";
        }
    }
    
    return "";
}

bool point_in_rect(int x, int y, SDL_Rect rect) {
    return x > rect.x && x < rect.x+rect.w && y > rect.y && y < rect.y+rect.h;
}

void start_game(Env *env) {
    env->game = new_game();

    if (env->disp_stage == SOLO) {
        env->BOT_P = NORTH_P;
        set_difficulty(env->selected_difficulty);
    } else {
        env->BOT_P = NO_PLAYER;
        if (env->SOCKET_P == SOUTH_P) { // NORTH_P always hosts
            env->serv_sockets = host(PORT);
            if (env->serv_sockets.s_sock == INVALID_SOCKET || env->serv_sockets.c_sock == INVALID_SOCKET) {
                printf("échec de host\n");
            }
            env->socket = env->serv_sockets.c_sock;
        } else if (env->SOCKET_P == NORTH_P) {
            env->socket = join("127.0.0.1", PORT); // TO DO : prompt IP
            if (env->socket == INVALID_SOCKET) {
                printf("échec de join\n");
            }
        }
    }

    // if (rand()%2 == 0) {  // random choice of the first player
    //     env->current_player = NORTH_P;
    // } else {
    //     env->current_player = SOUTH_P;
    // }

    env->current_player = NORTH_P;

    if (env->current_player != env->BOT_P && env->current_player != env->SOCKET_P) {
        enable_initial_pieces(env);
    }

    if (env->BOT_P != NO_PLAYER && env->swap_allowed) {
        sprintf(env->message, "Le bot ne sait pas utiliser le swap alors ne t'en sers pas ;)");
        render(env);
        SDL_RenderPresent(env->renderer);
        SDL_Delay(3000);
    } 
    sprintf(env->message, "%s, place tes pions !", player_name(env, env->current_player));
    env->disp_stage = PLACEMENT;
}

void init_title_sprites(Env *env) {
    TTF_Font *font;
    SDL_Color black = {0, 0, 0};

    font = TTF_OpenFont("assets/ubuntu.ttf", 100);

    env->title_sprites[0].texture = SDL_CreateTextureFromSurface(env->renderer, TTF_RenderUTF8_Solid(font, "Gygès", black));

    TTF_CloseFont(font);

    font = TTF_OpenFont("assets/ubuntu.ttf", 50);

    env->title_sprites[1].texture = SDL_CreateTextureFromSurface(env->renderer, TTF_RenderUTF8_Solid(font, "Swap autorisé :", black));
    
    TTF_CloseFont(font);

    env->title_sprites[2].texture = IMG_LoadTexture(env->renderer, "assets/solo.png");
    env->title_sprites[3].texture = IMG_LoadTexture(env->renderer, "assets/multi.png");
    env->title_sprites[4].texture = IMG_LoadTexture(env->renderer, "assets/go.png");

    for (int i = 0; i <= 4; i++) {
        SDL_QueryTexture(env->title_sprites[i].texture, NULL, NULL, &env->title_sprites[i].rect.w, &env->title_sprites[i].rect.h);
    }

    env->checkbox.textures[0] = IMG_LoadTexture(env->renderer, "assets/unchecked.png");
    env->checkbox.textures[1] = IMG_LoadTexture(env->renderer, "assets/checked.png");

    for (int i = 0; i < 2; i++) {
        if(!env->checkbox.textures[i]) {fprintf(stderr, "IMG_LoadTexture: %s\n", IMG_GetError());}
        SDL_QueryTexture(env->checkbox.textures[i], NULL, NULL, &env->checkbox.rect.w, &env->checkbox.rect.h);
    }

    place_title_sprites(env);
}

void init_solo_sprites(Env *env) {
    TTF_Font *font;
    SDL_Color black = {0, 0, 0};

    font = TTF_OpenFont("assets/ubuntu.ttf", 50);

    env->solo_sprites[0].texture = SDL_CreateTextureFromSurface(env->renderer, TTF_RenderUTF8_Solid(font, "Niveau de l'ordinateur :", black));
    SDL_QueryTexture(env->solo_sprites[0].texture, NULL, NULL, &env->solo_sprites[0].rect.w, &env->solo_sprites[0].rect.h);
    TTF_CloseFont(font);

    env->difficulties[0].texture = IMG_LoadTexture(env->renderer, "assets/facile.png");
    env->difficulties[1].texture = IMG_LoadTexture(env->renderer, "assets/moyen.png");
    env->difficulties[2].texture = IMG_LoadTexture(env->renderer, "assets/difficile.png");    

    for (int i = 0; i < 3; i++) {
        if(!env->difficulties[i].texture) {fprintf(stderr, "IMG_LoadTexture: %s\n", IMG_GetError());}
        SDL_QueryTexture(env->difficulties[i].texture, NULL, NULL, &env->difficulties[i].rect.w, &env->difficulties[i].rect.h);
    }

    place_solo_sprites(env);
}

void init_multi_sprites(Env *env) {
    env->multi_sprites[0].texture = IMG_LoadTexture(env->renderer, "assets/local.png");
    env->multi_sprites[1].texture = IMG_LoadTexture(env->renderer, "assets/host.png");
    env->multi_sprites[2].texture = IMG_LoadTexture(env->renderer, "assets/join.png");

    for (int i = 0; i <= 2; i++) {
        SDL_QueryTexture(env->multi_sprites[i].texture, NULL, NULL, &env->multi_sprites[i].rect.w, &env->multi_sprites[i].rect.h);
    }

    place_multi_sprites(env);
}

void init_pieces(Env *env) {
    env->pieces[0] = IMG_LoadTexture(env->renderer, "assets/piece1.png");
    env->pieces[1] = IMG_LoadTexture(env->renderer, "assets/piece2.png");
    env->pieces[2] = IMG_LoadTexture(env->renderer, "assets/piece3.png");

    env->picked_pieces[0] = IMG_LoadTexture(env->renderer, "assets/picked_piece1.png");
    env->picked_pieces[1] = IMG_LoadTexture(env->renderer, "assets/picked_piece2.png");
    env->picked_pieces[2] = IMG_LoadTexture(env->renderer, "assets/picked_piece3.png");

    for (int i = 0; i < 3; i++) {
        if(!env->pieces[i]) {fprintf(stderr, "IMG_LoadTexture: %s\n", IMG_GetError());}
        if(!env->picked_pieces[i]) {fprintf(stderr, "IMG_LoadTexture: %s\n", IMG_GetError());}
    }

    place_initial_pieces(env);
}

void init_background(Env *env) {
    env->background = IMG_LoadTexture(env->renderer, "assets/background.png");

    if(!env->background) {fprintf(stderr, "IMG_LoadTexture: %s\n", IMG_GetError());}
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
        if(!env->controls[i].texture) {fprintf(stderr, "IMG_LoadTexture: %s\n", IMG_GetError());}
        SDL_QueryTexture(env->controls[i].texture, NULL, NULL, &env->controls[i].rect.w, &env->controls[i].rect.h);
    }

    place_controls(env);
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

void calculate_cell_size(Env * env) {
    int window_w, window_h;
    int cell_size_w, cell_size_h;
    int tmp;

    SDL_GetWindowSize(env->window, &window_w, &window_h);

    cell_size_w = (window_w - MARGIN_RIGHT - MARGIN_LEFT)/DIMENSION;
    cell_size_h = (window_h - MARGIN_TOP - MARGIN_BOTTOM)/DIMENSION;

    if (cell_size_w < cell_size_h) {
        env->cell_size = cell_size_w;
        tmp = window_h - DIMENSION*env->cell_size; // available vertical space
        if (tmp > MARGIN_TOP*2) {
            env->margin_top = tmp/2;
        } else {
            env->margin_top = MARGIN_TOP;
        }
    } else {
        env->cell_size = cell_size_h;
        tmp = window_w - DIMENSION*env->cell_size; // available vertical space
        if (tmp > MARGIN_RIGHT*2) {
            env->margin_left = tmp/2;
        } else {
            env->margin_left = tmp-MARGIN_RIGHT;
        }
    }
}

void place_title_sprites(Env *env) {
    int window_w, window_h;

    SDL_GetWindowSize(env->window, &window_w, &window_h);

    env->title_sprites[0].rect.x = window_w/2 - env->title_sprites[0].rect.w/2;   // TITLE
    env->title_sprites[0].rect.y = window_h/10;

    env->title_sprites[1].rect.x = window_w/8;       // swap autorisé :
    env->title_sprites[1].rect.y = 2*window_h/5;

    env->checkbox.rect.x = env->title_sprites[1].rect.x + env->title_sprites[1].rect.w + 10;
    env->checkbox.rect.y = env->title_sprites[1].rect.y;
    env->checkbox.rect.h = env->title_sprites[1].rect.h;
    env->checkbox.rect.w = env->checkbox.rect.h;

    env->title_sprites[2].rect.x = window_w/2 - env->title_sprites[2].rect.w - 10;   // SOLO
    env->title_sprites[2].rect.y = window_h/2 - env->title_sprites[2].rect.h/2;

    env->title_sprites[3].rect.x = window_w/2 + 10;   // MULTI
    env->title_sprites[3].rect.y = window_h/2 - env->title_sprites[3].rect.h/2;

    place_solo_sprites(env);
    place_multi_sprites(env);

    env->title_sprites[4].rect.x = window_w/2 - env->title_sprites[4].rect.w/2; // GO
    env->title_sprites[4].rect.y = 4*window_h/5;
}

void place_solo_sprites(Env *env) {
    int window_w, window_h;

    SDL_GetWindowSize(env->window, &window_w, &window_h);

    env->solo_sprites[0].rect.x = window_w/2 - env->solo_sprites[0].rect.w/2;       // Niveau de l'ordi
    env->solo_sprites[0].rect.y = env->title_sprites[0].rect.y + env->title_sprites[0].rect.h + window_h/8;

    for (int i = 0; i < 3; i++) {
        env->difficulties[i].rect.x = window_w/2 + (i-1)*(env->difficulties[i].rect.w + 20) - env->difficulties[i].rect.w/2;
        env->difficulties[i].rect.y = env->solo_sprites[0].rect.y + env->solo_sprites[0].rect.h + 20;
    }
}

void place_multi_sprites(Env *env) {
    int window_w, window_h;

    SDL_GetWindowSize(env->window, &window_w, &window_h);

    env->multi_sprites[0].rect.x = window_w/2 - env->multi_sprites[0].rect.w/2;       // sur le même clavier
    env->multi_sprites[0].rect.y = env->title_sprites[0].rect.y + env->title_sprites[0].rect.h + 50;

    env->multi_sprites[1].rect.x = window_w/2 - env->multi_sprites[1].rect.w/2;       // host
    env->multi_sprites[1].rect.y = env->multi_sprites[0].rect.y + env->multi_sprites[0].rect.h + 20;

    env->multi_sprites[2].rect.x = window_w/2 - env->multi_sprites[2].rect.w/2;       // join
    env->multi_sprites[2].rect.y = env->multi_sprites[1].rect.y + env->multi_sprites[1].rect.h + 20;
}

void place_initial_pieces(Env *env) {
    for (int i = 0; i < DIMENSION; i++) {
        env->initial_pieces[i].rect.y = env->margin_top + DIMENSION*env->cell_size + 20;
        env->initial_pieces[i].rect.x = env->margin_left + DIMENSION*env->cell_size/2 + (i - DIMENSION/2)*(env->initial_pieces[0].rect.w+10);
        env->initial_pieces[i].rect.w = env->cell_size;
        env->initial_pieces[i].rect.h = env->cell_size;
    }
}

void enable_initial_pieces(Env *env) {
    for (int i = 0; i< DIMENSION; i++) {
        env->initial_pieces[i].size = i/2 + 1;
    }
}

void place_controls(Env *env) {
    int window_w, window_h;
    int mid_x, mid_y;

    SDL_GetWindowSize(env->window, &window_w, &window_h);
    mid_x = env->margin_left + DIMENSION*env->cell_size + MARGIN_RIGHT/2;
    mid_y = (window_h+env->margin_top/2)/2;

    env->controls[SOUTH].rect.x = mid_x - env->controls[SOUTH].rect.w/2;
    env->controls[SOUTH].rect.y = mid_y + env->controls[EAST].rect.h/2;

    env->controls[NORTH].rect.x = mid_x - env->controls[NORTH].rect.w/2;
    env->controls[NORTH].rect.y = mid_y - env->controls[EAST].rect.h/2 - env->controls[NORTH].rect.h;

    env->controls[EAST].rect.x = mid_x + env->controls[NORTH].rect.w/2;
    env->controls[EAST].rect.y = mid_y - env->controls[EAST].rect.h/2;

    env->controls[WEST].rect.x = mid_x - env->controls[NORTH].rect.w/2 - env->controls[EAST].rect.w;
    env->controls[WEST].rect.y = mid_y - env->controls[EAST].rect.h/2;

    env->controls[6].rect.x = mid_x - env->controls[6].rect.w/2; 
    env->controls[6].rect.y = env->controls[NORTH].rect.y - 10 - env->controls[6].rect.h; // swap

    env->controls[GOAL].rect.x = mid_x - env->controls[GOAL].rect.w/2;
    env->controls[GOAL].rect.y = env->controls[6].rect.y - 15 - env->controls[GOAL].rect.h;

    env->controls[5].rect.x = mid_x - env->controls[5].rect.w/2;
    env->controls[5].rect.y = env->controls[SOUTH].rect.y + env->controls[SOUTH].rect.h + 15; // cancel
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

void clear_screen(Env *env) {
    SDL_Rect rect;
    int window_w, window_h;

    SDL_GetWindowSize(env->window, &window_w, &window_h);

    SDL_RenderCopy(env->renderer, env->background, NULL, NULL); 

    if (env->disp_stage >= PLACEMENT) {
        rect.x = env->margin_left - 10;
        rect.y = env->margin_top - 10;
        rect.h = DIMENSION*env->cell_size + 20;
        rect.w = DIMENSION*env->cell_size + 20;
        SDL_SetRenderDrawColor(env->renderer,   139, 93, 46, 255);
        SDL_RenderFillRect(env->renderer, &rect);
    }    
}

void disp_sprites(Env *env, struct sprite sprites[], int len) {
    for (int i = 0; i < len; i++) {
        if (env->disp_stage == END) { // opaque background
            SDL_SetRenderDrawColor(env->renderer, 255, 255, 255, 50);
            SDL_RenderFillRect(env->renderer, &sprites[i].rect);
        }
        SDL_RenderCopy(env->renderer, sprites[i].texture, NULL, &sprites[i].rect);
    }
}

void disp_titlescreen(Env *env) {
    SDL_RenderCopy(env->renderer, env->title_sprites[0].texture, NULL, &env->title_sprites[0].rect);

    // SDL_RenderCopy(env->renderer, env->checkbox.textures[env->swap_allowed], NULL, &env->checkbox.rect);

    if (env->disp_stage == TITLE) {
        SDL_RenderCopy(env->renderer, env->title_sprites[2].texture, NULL, &env->title_sprites[2].rect);
        SDL_RenderCopy(env->renderer, env->title_sprites[3].texture, NULL, &env->title_sprites[3].rect);
    }

    else {
        if (env->disp_stage == SOLO) {
            SDL_RenderCopy(env->renderer, env->solo_sprites[0].texture, NULL, &env->solo_sprites[0].rect);

            for (int i = 0; i < 3; i++) {
                if (env->selected_difficulty == i) {
                    SDL_SetTextureAlphaMod(env->difficulties[i].texture, 255);
                } else {
                    SDL_SetTextureAlphaMod(env->difficulties[i].texture, 150);
                }
                SDL_RenderCopy(env->renderer, env->difficulties[i].texture, NULL, &env->difficulties[i].rect);
            }

            SDL_RenderCopy(env->renderer, env->title_sprites[4].texture, NULL, &env->title_sprites[4].rect); 
        }
        if (env->disp_stage == MULTI) {
            disp_sprites(env, env->multi_sprites, 3);
        }
        
    }
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

void disp_winner_piece(Env *env) {
    size winner_piece = NONE;
    int count, window_w;
    SDL_Rect rect;

    SDL_GetWindowSize(env->window, &window_w, NULL);

    for (size s = ONE; s <= THREE; s++) { // find the winner's piece size (API's lack)
        count = 0;

        for (int line = 0; line < DIMENSION; line++) {
            for (int column = 0; column < DIMENSION; column++) {
                if (get_piece_size(env->game, line, column) == s) {
                    count++;
                }
            }
        }

        if (count < NB_INITIAL_PIECES*NB_PLAYERS) {
            winner_piece = s;
        }
    }
    
    if (winner_piece == NONE) {
        return;
    }

    rect.h = env->cell_size;
    rect.w = env->cell_size;
    rect.x = env->margin_left + DIMENSION*env->cell_size/2 - rect.w/2;

    if (get_winner(env->game) == SOUTH_P) {
        rect.y = env->margin_top - rect.h - 10;
    } else {
        rect.y = env->margin_top + env->cell_size*DIMENSION + 10;
    }

    SDL_RenderCopy(env->renderer, env->pieces[winner_piece-1], NULL, &rect);
}

int sprite_clicked(int x, int y, struct sprite sprites[], int len) {
    for (int i = 0; i < len; i++) {
        if (point_in_rect(x, y, sprites[i].rect)) {
            return i;
        }
    }
    return -1;
}

int initial_piece_clicked(Env *env, int x, int y) {
    for (int i = 0; i <= DIMENSION; i++) {
        if (point_in_rect(x, y, env->initial_pieces[i].rect)) {
            return i;
        }
    }
    return -1;
}

position position_clicked(Env *env, int x, int y) {
    position response = {-1, -1};

    if (x > env->margin_left && x < env->margin_left+DIMENSION*env->cell_size && y > env->margin_top && y < env->margin_top+DIMENSION*env->cell_size) {
        response.column = (x - env->margin_left)/env->cell_size;
        response.line = DIMENSION-1 - (y - env->margin_top)/env->cell_size;
    }

    return response;
}

void menu_choices(Env *env, SDL_Event *event) {

    // if (point_in_rect(event->button.x, event->button.y, env->checkbox.rect)) {
    //     env->swap_allowed = !env->swap_allowed;
    // }

    if (env->disp_stage == TITLE) {
        if (point_in_rect(event->button.x, event->button.y, env->title_sprites[2].rect)) {
            env->disp_stage = SOLO;
        }
        if (point_in_rect(event->button.x, event->button.y, env->title_sprites[3].rect)) {
            env->disp_stage = MULTI;
        }
        return;
    }

    if (env->disp_stage == SOLO) {
        for (int i = 0; i < 3; i++) {
            if (point_in_rect(event->button.x, event->button.y, env->difficulties[i].rect)) {    
                env->selected_difficulty = i;
            }
        }
    }

    if (env->disp_stage == MULTI) {
        if (point_in_rect(event->button.x, event->button.y, env->multi_sprites[0].rect)) {    
            env->SOCKET_P = NO_PLAYER;
            start_game(env);
        }
        if (point_in_rect(event->button.x, event->button.y, env->multi_sprites[1].rect)) {    
            env->SOCKET_P = SOUTH_P;
            start_game(env);
        }
        if (point_in_rect(event->button.x, event->button.y, env->multi_sprites[2].rect)) {    
            env->SOCKET_P = NORTH_P;
            start_game(env);
        }
    }   
}

void drag_initial_pieces(Env *env, SDL_Event *event) {
    piece s_piece;
    int piece_clicked;
    position pos_clicked;
    bool done;

    if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == 1) { // drag
        piece_clicked = initial_piece_clicked(env, event->button.x, event->button.y);
        if (piece_clicked > -1) {
            env->dragging_piece = piece_clicked;
        }
    }
    else if (env->dragging_piece > -1 && event->type == SDL_MOUSEBUTTONUP && event->button.button == 1) { // drop
        pos_clicked = position_clicked(env, event->button.x, event->button.y);
        if ((env->current_player == NORTH_P && pos_clicked.line == DIMENSION-1) || (env->current_player == SOUTH_P && pos_clicked.line == 0)) {
            if (place_piece(env->game, env->initial_pieces[env->dragging_piece].size, env->current_player, pos_clicked.column) == OK) {
                if (env->SOCKET_P != NO_PLAYER) {
                    s_piece.size = env->initial_pieces[env->dragging_piece].size;
                    s_piece.position.line = pos_clicked.line;
                    s_piece.position.column = pos_clicked.column;
                    if (send(env->socket, &s_piece, sizeof(piece), 0) == SOCKET_ERROR)
                        printf("L'envoi a échoué\n");
                }

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
                        if (env->current_player != env->BOT_P && env->current_player != env->SOCKET_P) {
                            enable_initial_pieces(env);
                        }
                    }
                }
            }
        }
        env->dragging_piece = -1;       
    }
}

void choose_piece_to_pick(Env *env, SDL_Event *event) {
    position pos_clicked;
    piece s_piece;

    pos_clicked = position_clicked(env, event->button.x, event->button.y);
    if (pick_piece(env->game, env->current_player, pos_clicked.line, pos_clicked.column) == OK && env->SOCKET_P != NO_PLAYER) {
        s_piece.position.line = pos_clicked.line;
        s_piece.position.column = pos_clicked.column;
        if (send(env->socket, &s_piece, sizeof(piece), 0) == SOCKET_ERROR)
            printf("L'envoi a échoué\n");
    }
}

void choose_direction(Env *env, SDL_Event *event) {
    int dir_clicked;
    direction s_direction;

    dir_clicked = sprite_clicked(event->button.x, event->button.y, env->controls, 7);

    if (dir_clicked == 5) { // cancel
        cancel_step(env->game);
    }
    else if (dir_clicked == 6) { // swap
        if (movement_left(env->game) == 0 && env->swap_allowed) {
            env->dragging_piece = get_piece_size(env->game, picked_piece_line(env->game), picked_piece_column(env->game));
            sprintf(env->message, "Pose la pièce sur le plateau.");
            return;
        }
    }
    else if (dir_clicked != -1) {
        move_piece(env->game, dir_clicked);
        if (env->SOCKET_P != NO_PLAYER) {
            s_direction = dir_clicked;
            if (send(env->socket, &s_direction, sizeof(direction), 0) == SOCKET_ERROR)
                printf("L'envoi a échoué\n");
        }
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

bool end_choices(Env *env, SDL_Event *event) {
    int button_clicked = sprite_clicked(event->button.x, event->button.y, env->end_buttons, 3);

    if (button_clicked == 0) {
        env->disp_stage = TITLE;
    } else if (button_clicked == 1) {
        start_game(env);
    } else if (button_clicked == 2) {
        return true;
    }

    return false;
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
