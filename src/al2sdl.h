#ifndef AL2SDL_H_
#define AL2SDL_H_

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

#define font 0

#define KEY_ESC     0
#define KEY_1       1
#define KEY_2       2
#define KEY_3       3
#define KEY_4       4

extern int mouse_b;
extern int key[256];



int al2sdl_color( int alc );

int MAX( int a, int b );
int ABS( int value );

void rest( int millisecs );

void clear_to_color( SDL_Surface *dest, int colour );

void destroy_bitmap( SDL_Surface *img );

int getpixel( SDL_Surface *src, int x, int y );
void putpixel( SDL_Surface *dest, int x, int y, int color );


void rect( SDL_Surface *dest, int x, int y, int w, int h, int color );
void rectfill( SDL_Surface *dest, int x, int y, int w, int h, int color );

void stretch_blit( SDL_Surface *dest, SDL_Surface *src, int srcx, int srcy, int srcw, int srch, int destx, int desty, int destw, int desth);
void blit( SDL_Surface *src, SDL_Surface *dest, int srcx, int srcy, int destx, int desty, int width, int height);

void textout_ex(SDL_Surface *dest, void *foo, const char *msg, int x, int y, int colour, int bgcolour );
void textprintf_ex(SDL_Surface *dest, void *foo, int x, int y, int color, int bg, const char *fmt, ...);
void textout_centre_ex(SDL_Surface *dest, void *foo, const char *s, int x, int y, int color, int bg);
void textprintf_centre_ex(SDL_Surface *dest, void *foo, int x, int y, int color, int bg, const char *fmt, ...);


void al2sdl_poll();
void init_al2sdl();
void al2sdl_clear_input();

#endif
