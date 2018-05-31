#include <SDL/SDL.h>

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "al2sdl.h"
#include "log.h"
#include "DarkLight.h"
#include "screen.h"

#define MOUSE_H_SPEED 16
#define MOUSE_V_SPEED 16

int mouse_b = 0;
int key[256];

unsigned char dpad = 0;

int gp2x_l_down = false, gp2x_r_down = false;

SDL_Joystick *joy;

#define GP2X_BUTTON_UP              (0)
#define GP2X_BUTTON_DOWN            (4)
#define GP2X_BUTTON_LEFT            (2)
#define GP2X_BUTTON_RIGHT           (6)
#define GP2X_BUTTON_UPLEFT          (1)
#define GP2X_BUTTON_UPRIGHT         (7)
#define GP2X_BUTTON_DOWNLEFT        (3)
#define GP2X_BUTTON_DOWNRIGHT       (5)
#define GP2X_BUTTON_CLICK           (18)
#define GP2X_BUTTON_A               (12)
#define GP2X_BUTTON_B               (13)
#define GP2X_BUTTON_X               (15)
#define GP2X_BUTTON_Y               (14)
#define GP2X_BUTTON_L               (11)
#define GP2X_BUTTON_R               (10)
#define GP2X_BUTTON_START           (8)
#define GP2X_BUTTON_SELECT          (9)
#define GP2X_BUTTON_VOLUP           (16)
#define GP2X_BUTTON_VOLDOWN         (17)

int sdl2al_color( int sdlc )
{
    int col;

    int c0,c1,c2,c3,c4;

    c0 = SDL_MapRGB( screen->format, 0,0,0);
    c1 = SDL_MapRGB( screen->format, 86,87,194);
    c2 = SDL_MapRGB( screen->format, 170,170,255);
    c3 = SDL_MapRGB( screen->format, 255,255,255);
    c4 = SDL_MapRGB( screen->format, 174,105,174);

    col = 0;
    if ( sdlc == c0 ) col = 0;
    if ( sdlc == c1 ) col = 1;
    if ( sdlc == c2 ) col = 2;
    if ( sdlc == c3 ) col = 3;
    if ( sdlc == c4 ) col = 4;

    return col;
}

int al2sdl_color( int alc )
{

    int col;
    switch ( alc )
    {
    case 16:
    case 0:
        col = SDL_MapRGB( screen->format, 0,0,0) ;
        break;
    case 1:
        col = SDL_MapRGB( screen->format, 86,87,194) ;
        break;
    case 2:
        col = SDL_MapRGB( screen->format, 170,170,255) ;
        break;
    case 15:
    case 3:
        col = SDL_MapRGB( screen->format, 255,255,255) ;
        break;
    case 4:
        col = SDL_MapRGB( screen->format, 174,105,174) ;
        break;

    default:
        col = SDL_MapRGB( screen->format, 255,0,0) ;
        break;
    }

    return col;
}

SDL_Color translate_color(Uint32 int_color)  //Change from an "int color" to an SDL_Color
{
    /*
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    SDL_Color color={(int_color & 0x00ff0000)/0x10000,(int_color &
                     0x0000ff00)/0x100,(int_color & 0x000000ff),0
                    };
#else
    SDL_Color color={(int_color & 0x000000ff),(int_color &
                     0x0000ff00)/0x100,(int_color & 0x00ff0000)/0x10000,0
                    };
#endif
*/
    SDL_Color color;
    Uint8 r,g,b;

    SDL_GetRGB( int_color, screen->format, &r, &g, &b );
    color.r = r;
    color.g = g;
    color.b = b;

    return color;
}


int MAX( int a, int b )
{
    return a<b?b:a;
}

int ABS( int value )
{
    return value<0?-value:value;
}


void rest( int millisecs )
{
    int start = SDL_GetTicks();
    while( SDL_GetTicks() < start + millisecs )
    {
        al2sdl_poll();
    }
    //SDL_Delay( millisecs );
}


void clear_to_color( SDL_Surface *dest, int colour )
{
    SDL_Rect r;
    r.x = 0;
    r.y = 0;
    r.w = dest->w;
    r.h = dest->h;
    SDL_FillRect( dest, &r, colour); //al2sdl_color(colour) );
}


void destroy_bitmap( SDL_Surface *img )
{
    SDL_FreeSurface( img );
}


int getpixel( SDL_Surface *src, int x, int y )
{
    if ( x < 0 || y < 0 || x > src->w || y > src->h ) return -1;
    int c;

    SDL_LockSurface(src);
    int bpp = src->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)src->pixels + y * src->pitch + x * bpp;

    switch (bpp)
    {
    case 1:
        c= *p;
        break;
    case 2:
        c= *(Uint16 *)p;
        break;
    case 3:
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
            c= p[0] << 16 | p[1] << 8 | p[2];
        else
            c= p[0] | p[1] << 8 | p[2] << 16;
        break;
    case 4:
        c= *(Uint32 *)p;
        break;
    default:
        c = 0;       /* shouldn't happen, but avoids warnings */
        break;
    }
    SDL_UnlockSurface(src);
    return c;
}

void putpixel( SDL_Surface *dest, int x, int y, int color )
{
    if ( x < 0 || y < 0 || x > dest->w || y > dest->h ) return;

    SDL_LockSurface(dest);
    int bpp = dest->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)dest->pixels + y * dest->pitch + x * bpp;

    switch (bpp)
    {
    case 1:
        *p = color;
        break;

    case 2:
        *(Uint16 *)p = color;
        break;

    case 3:
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
        {
            p[0] = (color >> 16) & 0xff;
            p[1] = (color >> 8) & 0xff;
            p[2] = color & 0xff;
        }
        else
        {
            p[0] = color & 0xff;
            p[1] = (color >> 8) & 0xff;
            p[2] = (color >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32 *)p = color;
        break;
    }
    SDL_UnlockSurface(dest);
}



void rect( SDL_Surface *dest, int x1, int y1, int x2, int y2, int color )
{
    SDL_Rect r;

    // left
    r.x = x1;
    r.y = y1;
    r.w = 1;
    r.h = y2-y1;
    SDL_FillRect( dest, &r, color );

    // top
    r.w = x2-x1;
    r.h = 1;
    SDL_FillRect( dest, &r, color );

    // bottom
    r.y = y2;
    SDL_FillRect( dest, &r, color );

    // right
    r.x = x2;
    r.y = y1;
    r.w = 1;
    r.h = y2-y1+1;
    SDL_FillRect( dest, &r, color );

}

void rectfill( SDL_Surface *dest, int x, int y, int w, int h, int color )
{
    SDL_Rect r;
    r.x = x;
    r.y = y;
    r.w = w-x+1;
    r.h = h-y+1;
    SDL_FillRect( dest, &r, color ); //al2sdl_color(color) );
}


void stretch_blit( SDL_Surface *dest, SDL_Surface *src, int srcx, int srcy, int srcw, int srch, int destx, int desty, int destw, int desth)
{
    blit( src, dest, srcx, srcy, destx, desty, srcw, srch );
}

void blit( SDL_Surface *src, SDL_Surface *dest, int srcx, int srcy, int destx, int desty, int width, int height)
{
    SDL_Rect sr, dr;
    sr.x = srcx;
    sr.y = srcy;
    sr.w = width;
    sr.h = height;

    dr.x = destx;
    dr.y = desty;
//    dr.w = width;
//    dr.h = height;

    SDL_BlitSurface( src, &sr, dest, &dr );

}


void textout_ex(SDL_Surface *dest, void *foo, const char *msg, int x, int y, int colour, int bgcolour )
{
    if (!msg) return;
    if (strlen(msg) == 0 ) return;

    SDL_Color foregroundColor = translate_color(colour);

    SDL_Surface* textSurface =  TTF_RenderText_Solid(sdlfont,msg, foregroundColor);
    SDL_Surface* textbg = NULL;
    if ( bgcolour >= 0 )
    {
        textbg = SDL_CreateRGBSurface( surface_type,textSurface->w,textSurface->h,16,0,0,0,0 );
        clear_to_color( textbg, al2sdl_color(bgcolour) );
    }

    SDL_Rect textLocation = { x, y, 0, 0 };
    if ( textbg ) SDL_BlitSurface(textbg, NULL, dest, &textLocation );
    SDL_BlitSurface(textSurface, NULL, dest, &textLocation);
    SDL_FreeSurface(textSurface);
    if ( textbg ) SDL_FreeSurface(textbg);
}

void textprintf_ex(SDL_Surface *dest, void *foo, int x, int y, int color, int bg, const char *fmt, ...)
{
    va_list ptr;
    char *buff;
    buff = (char*)malloc( 1024 * sizeof(char) );
    va_start(ptr, fmt);
    vsprintf(buff, fmt, ptr);

    textout_ex( dest, 0, buff, x,y, color, bg );

    free( buff );
    va_end(ptr);
}


void textout_centre_ex(SDL_Surface *dest, void *foo, const char *str, int x, int y, int color, int bg)
{
    if (!str) return;
    if (strlen(str) == 0 ) return;

    SDL_Color foregroundColor = translate_color(color);

    SDL_Surface* textSurface =  TTF_RenderText_Solid(sdlfont,str, foregroundColor);
    SDL_Surface* textbg = NULL;
    if ( bg >= 0 )
    {
        textbg = SDL_CreateRGBSurface( surface_type,textSurface->w,textSurface->h,16,0,0,0,0 );
        clear_to_color( textbg, al2sdl_color(bg) );
    }

    SDL_Rect textLocation = { x - (textSurface->w/2), y, 0, 0 };
    if ( textbg ) SDL_BlitSurface(textbg, NULL, dest, &textLocation );
    SDL_BlitSurface(textSurface, NULL, dest, &textLocation);
    SDL_FreeSurface(textSurface);
    if ( textbg ) SDL_FreeSurface(textbg);
}

void textprintf_centre_ex(SDL_Surface *dest, void *foo, int x, int y, int color, int bg, const char *fmt, ...)
{
    va_list ptr;
    char *buff;
    buff = (char*)malloc( 1024 * sizeof(char) );
    va_start(ptr, fmt);
    vsprintf(buff, fmt, ptr);

    textout_centre_ex( dest, 0, buff, x,y, color, bg );

    free( buff );
    va_end(ptr);
}



Uint32 al2sdl_polltimer(Uint32 interval, void *param)
{
    al2sdl_poll();
    return(interval);
}


void al2sdl_poll()
{

    SDL_Event event;
    dpad = 0;
    while ( SDL_PollEvent(&event) )
    {
        switch (event.type)
        {
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
            {
            case SDLK_ESCAPE:
                key[KEY_ESC] = true;
                break;
            //case SDLK_z:
            case SDLK_LCTRL:
                mouse_b = 1;
                break;
            //case SDLK_x:
            case SDLK_LALT:
                mouse_b = 2;
                break;

            //case SDLK_TAB:
            case SDLK_LSHIFT:
                mouse_b = 3;
                break;

            case SDLK_LEFT:
                curx -= 1;
                dpad = 1;
                break;
            case SDLK_RIGHT:
                curx += 1;
                dpad = 2;
                break;
            case SDLK_UP:
                cury -= 1;
                dpad = 3;
                break;
            case SDLK_DOWN:
                cury += 1;
                dpad = 4;
                break;

            case SDLK_PAGEUP:
            case SDLK_TAB:
                ChangeVolume( 8 );
                break;
            case SDLK_PAGEDOWN:
            case SDLK_BACKSPACE:
                ChangeVolume( -8 );
                break;

            default:
                break;
            }
            break;
        case SDL_KEYUP:
            switch (event.key.keysym.sym)
            {
            case SDLK_ESCAPE:
                key[KEY_ESC] = false;
                break;
            //case SDLK_z:
            //case SDLK_x:
            case SDLK_LALT:
            case SDLK_LCTRL:
                mouse_b = 0;
			break;
            //case SDLK_z:
            //case SDLK_x:
            case SDLK_UP:
            case SDLK_DOWN:
            case SDLK_LEFT:
            case SDLK_RIGHT:
                dpad = 0;
			break;
            default:
                break;
            }
            break;
		#ifdef NOJOY
        case SDL_JOYBUTTONDOWN:
            if ( event.jbutton.button == GP2X_BUTTON_A )
                mouse_b = 1;
            if ( event.jbutton.button == GP2X_BUTTON_B )
                mouse_b = 2;
            if ( event.jbutton.button == GP2X_BUTTON_L )
                gp2x_l_down = true;
            if ( event.jbutton.button == GP2X_BUTTON_R )
                gp2x_r_down = true;

            if ( event.jbutton.button == GP2X_BUTTON_RIGHT )
                curx += 1;
            if ( event.jbutton.button == GP2X_BUTTON_LEFT )
                curx -= 1;
            if ( event.jbutton.button == GP2X_BUTTON_UP )
                cury -= 1;
            if ( event.jbutton.button == GP2X_BUTTON_DOWN )
                cury += 1;

            if ( event.jbutton.button == GP2X_BUTTON_SELECT )
                mouse_b = 3;

            if ( event.jbutton.button == GP2X_BUTTON_VOLUP )
                ChangeVolume( 8 );
            if ( event.jbutton.button == GP2X_BUTTON_VOLDOWN )
                ChangeVolume( -8 );

            break;
        case SDL_JOYBUTTONUP:
            if ( event.jbutton.button == GP2X_BUTTON_A || event.jbutton.button == GP2X_BUTTON_B )
                mouse_b = 0;

            if ( event.jbutton.button == GP2X_BUTTON_L )
                gp2x_l_down = false;
            if ( event.jbutton.button == GP2X_BUTTON_R )
                gp2x_r_down = false;
            break;
		#endif
        default:
            break;
        }

    }


    if ( curx < 0 ) curx = 0;
    if ( curx >= TILES_X ) curx = TILES_X-1;
    if ( cury < 0 ) cury = 0;
    if ( cury >= TILES_Y ) cury = TILES_Y-1;



    if ( gp2x_l_down && gp2x_r_down )
        key[KEY_ESC] = true;

}



void init_al2sdl()
{
    write_to_log("Initiating al2sdl");

    //write_to_log("Starting al2sdl timer");
    //SDL_AddTimer( 100, al2sdl_polltimer, NULL );

    write_to_log("Initiating joystick");
    joy = SDL_JoystickOpen(0);

    write_to_log("Done with init_al2sdl");
}



void al2sdl_clear_input()
{
    mouse_b = 0;
    for ( int i = 0; i < 256; i++ )
        key[i] = false;
    gp2x_l_down = false;
    gp2x_r_down = false;
}
