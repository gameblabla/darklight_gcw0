// Free_dungeons (.H)

#ifndef DARKLIGHT_H_
#define DARKLIGHT_H_

#include <stdio.h>
#include <stdlib.h>

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mixer.h>

#ifdef PLATFORM_GP2X
#include <unistd.h>
#endif // PLATFORM_GP2X

#include "sound.h"

#define GAME_NAME       "DarkLight Battles SDL2X"
#define GAME_VERSION    "0.1"
#define GAME_AUTHOR     "Richard Phipps. SDL GP2X adaption by Peter Sundling"
#define GAME_DATE       "21.01.08"

#define NO              0
#define YES             1

#define MAX_PLAYERS     2

extern int surface_type;

extern TTF_Font* sdlfont;

extern SDL_Surface *screen;

//PALETTE ingame_palette;
//BITMAP *tiles;
extern SDL_Surface *tiles;
extern SDL_Surface *title_image;
extern SDL_Surface *title_options;
extern SDL_Surface *title_text;



extern SDL_Surface *balance;
extern SDL_Surface *balancebg;

extern SDL_Surface *cursor;
extern SDL_Surface *cmove;
//BITMAP *small_buffer;
//BITMAP *large_buffer;
extern SDL_Surface *small_buffer;
//extern SDL_Surface *large_buffer;

//BITMAP *map;
//BITMAP *army;
extern SDL_Surface *map;
extern SDL_Surface *army;

extern Mix_Chunk *sfx_hit;
extern Mix_Chunk *sfx_arrow;
extern Mix_Chunk *sfx_fireball;
extern Mix_Chunk *sfx_spawn;
extern Mix_Chunk *sfx_castle;
extern Mix_Chunk *sfx_step;

extern Mix_Chunk *sfx_chains;
extern Mix_Chunk *sfx_gameover;

extern Mix_Music *mus_title;
extern Mix_Music *mus_balanced;
extern Mix_Music *mus_light;
extern Mix_Music *mus_dark;

typedef struct s_game
{
    int side;
    int turn;
    int units_who_can_move;
    int winning_side;
};

extern s_game game;

typedef struct s_player
{
    int cpu;
};

extern s_player player[MAX_PLAYERS];

extern int curx, cury, ocurx, ocury;

void startup_game(void);
void main_loop(void);
void shutdown_game(void);

#endif
