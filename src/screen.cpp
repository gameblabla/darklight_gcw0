// Screen (.c)

//#include <allegro.h>
#include <SDL/SDL.h>
#include <string.h>
#include <stdarg.h>

#include "DarkLight.h"
#include "screen.h"
#include "unit.h"

#include "al2sdl.h"

int cursor_frame = 0;
int cursor_frame_timer = 0;

char message[128];

extern unsigned char dpad;

void draw_screen(void)
{
    /*
     // Stretch from small to large buffer.
     stretch_blit(small_buffer, large_buffer, 0, 0, small_buffer->w, small_buffer->h,
     0, 0, large_buffer->w, large_buffer->h);

     // Then blit large buffer to screen.
     blit(large_buffer, screen, 0, 0, 0, 0, large_buffer->w, large_buffer->h);
     */

    SDL_Rect r;
    r.x = 0;
    r.y = 0;
    r.w = 320;
    r.h = 240;
    SDL_BlitSurface( small_buffer, &r, screen, &r );
    SDL_Flip( screen );
}

void draw_tile(int x, int y, int tile)
{
    int tile_x, tile_y;

    tile_x = (tile / 4) * 16;
    tile_y = (tile % 4) * 16;

    /*
     for (ty = 0 ; ty < 16 ; ty++)
     {
      for (tx = 0 ; tx < 16 ; tx++)
      {
       c = getpixel(tiles, tx + tile_x, ty + tile_y);

       if (c == 4) continue; // Skip masked pixel colour.
       putpixel(small_buffer, x + tx, y + ty, c);
      }
     }
     */

    blit( tiles, small_buffer, tile_x,tile_y, x,y, 16,16 );

}

void draw_map(void)
{
    int x, y, tile;

    //redraw_cursor_bg();

    for (y = 0 ; y < TILES_Y ; y++)
    {
        for (x = 0 ; x < TILES_X ; x++)
        {
            tile = getpixel(map, x, y);
            draw_tile(x * 16, 14+(y * 16), tile);

            if (unit_map[x][y].attack != 0)
            {
                /*
                // Blend units with no movement points left
                if ( unit_map[x][y].gfx / 4 == game.side && unit_map[x][y].movement == 0 )
                {
                    SDL_SetAlpha( tiles, SDL_SRCALPHA, 128 );
                    draw_tile(x * 16, (y * 16)+14, unit_map[x][y].gfx);
                    SDL_SetAlpha( tiles, SDL_SRCALPHA, 255 );
                }
                else
                    draw_tile(x * 16, (y * 16)+14, unit_map[x][y].gfx);
                */

                // Selected?
                if ( x == selected.x && y == selected.y )
                    rectfill(small_buffer, x * 16, (y * 16)+14, (x * 16) + 15, (y * 16) + 14 + 15, al2sdl_color(2));

                if( unit_map[x][y].gfx / 4 == game.side && unit_map[x][y].movement <= 0 ) SDL_SetAlpha( tiles, SDL_SRCALPHA, 96 );
                draw_tile(x * 16, (y * 16)+14, unit_map[x][y].gfx);
                SDL_SetAlpha( tiles, SDL_SRCALPHA, 255 );
            }
        }
    }


    draw_cursor();

    // Messages
    rectfill( small_buffer, 0,0,320,12, al2sdl_color(3) );
    rectfill( small_buffer, 0,13,320,13, al2sdl_color(0) );
    textout_centre_ex( small_buffer, 0, message, 160,1, 0,3 );

    // Balance meter
    int dsize = 0, lsize = 0;

    for (y = 0 ; y < TILES_Y ; y++)
    {
        for (x = 0 ; x < TILES_X ; x++)
        {
            if (unit_map[x][y].gfx == -1) continue;
            if ( (unit_map[x][y].gfx < SOLDIER + 4 ) )
                dsize++;
            else
                lsize++;
        }
    }

    /*
    int pos = (((float)dsize / (dsize+lsize)) * 320);
    blit( balancebg, small_buffer, 0,0, 0,206, 320,22 );
    blit( balance, small_buffer, 0,0, pos-16,206, 32,22 );
    */

    int pos = 12+( (1-((float)dsize / (dsize+lsize))) * 298 );
    blit( balancebg, small_buffer, pos,0, 0,206, 320,22 );

    int frame = (int)((1-((float)dsize / (dsize+lsize))) * 7)*22;
    pos = 12 + ( (((float)dsize / (dsize+lsize))) * 298 );
    blit( balance, small_buffer, frame,0, pos-11,206, 22,22 );


    // Background for status text
    rectfill(small_buffer, 0, small_buffer->h - 11, small_buffer->w, small_buffer->h, al2sdl_color(3) );

    /*
     // Fill out the extra space on the GP2X screen
     rectfill( small_buffer, 0,13,319,13, al2sdl_color(0) );
     rectfill( small_buffer, 0,207,319,207, al2sdl_color(0) );
     rectfill( small_buffer, 0,208,319,239, al2sdl_color(3) );


     if (game.side == 0)
     {
      //textprintf_centre_ex(small_buffer, font, small_buffer->w / 2, small_buffer->h * 0.4, 0, 3,"** The Dark **");
      textprintf_centre_ex(small_buffer, 0, small_buffer->w / 2, 210, 0, 3,"** The Dark **");
     }
     else
     {
      //textprintf_centre_ex(small_buffer, font, small_buffer->w / 2, small_buffer->h * 0.4, 0, 3,"-- The Light --");
      textprintf_centre_ex(small_buffer, 0, small_buffer->w / 2, 210, 0, 3,"-- The Light --");
     }

      textprintf_ex( small_buffer, 0, 0,0, 0,3, "Units left: %d", game.units_who_can_move );
    */

}

void create_map(void)
{
    int x, y, r, c;
    SDL_Surface *temp;

    clear_to_color(map, 8);
    temp = SDL_CreateRGBSurface( surface_type,map->w, map->h, 16, 0,0,0,0);

    for (r = 0 ; r < 50 ; r++)
    {
        putpixel(map, rand() % TILES_X, rand() % TILES_Y, (rand() % 3) + 8);
    }

// Grow terrain..
    for (r = 0 ; r < 15 ; r++)
    {
        blit(map, temp, 0, 0, 0, 0, map->w, map->h);

        for (y = 0 ; y < TILES_Y ; y++)
        {
            for (x = 0 ; x < TILES_X ; x++)
            {
                c = getpixel(temp, x, y);

                if (c == 8) continue;

                if (c == 9 || (c == 10 && rand() % 2 == 0))
                {
                    if (rand() % 2 == 0)
                    {
                        putpixel(map, x + (rand() % 2) - 1, y, c);
                    }
                    else
                    {
                        putpixel(map, x, y + (rand() % 2) - 1, c);
                    }
                }
            }
        }
    }

// Add rocks.
    for (r = 0 ; r < 8 ; r++)
    {
        putpixel(map, rand() % TILES_X, rand() % TILES_Y, 12);
    }

// Add free castles.
    for (r = 0 ; r < 8 ; r++)
    {
        x = rand() % (TILES_X - 9) + 4;
        y = rand() % TILES_Y;

        //rectfill(map, x - 1, y - 1, x + 1, y + 1, 8);
        putpixel(map, x, y, 13);
    }

    destroy_bitmap(temp);

// Add armies.
    add_armies();
}

void add_armies(void)
{
    int dy, ly, w;

    dy = rand() % (TILES_Y - 2) + 1;
    ly = rand() % (TILES_Y - 2) + 1;

    w = TILES_X - 2;

// Dark Castle
    rectfill(map, 0, dy - 1, 2, dy + 1, 8);
    putpixel(map, 1, dy, 14);

// Soldiers
    unit_map[2][dy - 1] = unit[SOLDIER];
    unit_map[2][dy] = unit[SOLDIER];
    unit_map[2][dy + 1] = unit[SOLDIER];

// Knights
    unit_map[1][dy - 1] = unit[KNIGHT];
    unit_map[1][dy + 1] = unit[KNIGHT];

// Archers
    unit_map[0][dy - 1] = unit[ARCHER];
    unit_map[0][dy + 1] = unit[ARCHER];

// Mage
    unit_map[0][dy] = unit[MAGE];


// Light Castle
    rectfill(map, w - 1, ly - 1, w + 1, ly + 1, 8);
    putpixel(map, w, ly, 15);

    // Soldiers
    unit_map[w - 1][ly - 1] = unit[SOLDIER + 4];
    unit_map[w - 1][ly] = unit[SOLDIER + 4];
    unit_map[w - 1][ly + 1] = unit[SOLDIER + 4];


// Knights
    unit_map[w][ly - 1] = unit[KNIGHT + 4];
    unit_map[w][ly + 1] = unit[KNIGHT + 4];

// Archers
    unit_map[w + 1][ly - 1] = unit[ARCHER + 4];
    unit_map[w + 1][ly + 1] = unit[ARCHER + 4];

// Mage
    unit_map[w + 1][ly] = unit[MAGE + 4];
}

void draw_status_text(int x, int y)
{
    if( player[0].cpu == YES && player[1].cpu == YES )
    {

        return;
    }

    char txt[40];
    int t, u;

    if (y > 11)
    {
        sprintf(txt, "%d units left to move. (A) to End Turn.", game.units_who_can_move);

        rectfill(small_buffer, 0, small_buffer->h - 11, small_buffer->w, small_buffer->h, al2sdl_color(1) );
        //textout_ex(small_buffer, font, txt, 0, small_buffer->h - 8, 0, 1);
        textout_ex(small_buffer, 0, txt, 0, small_buffer->h - 11, 0, 1);
        return;
    }


    if (selected.x != -1)
    {
        x = selected.x;
        y = selected.y;
    }

    t = getpixel(map, x, y);

    if (t == 8) sprintf(txt, "Terrain: Plain");
    if (t == 9) sprintf(txt, "Terrain: Grasslands");
    if (t == 10) sprintf(txt, "Terrain: Forest");
    if (t == 11) sprintf(txt, "Terrain: Water");
    if (t == 12) sprintf(txt, "Terrain: Rock");
    if (t == 13) sprintf(txt, "Terrain: Neutral Castle");
    if (t == 14) sprintf(txt, "Terrain: Dark Castle");
    if (t == 15) sprintf(txt, "Terrain: Light Castle");

    u = unit_map[x][y].movement;
    if (u == -1) u = 0;

    if (unit_map[x][y].gfx == 0) sprintf(txt, "Dark Soldier.  Moves Left: %d  (B): Stats",
                                             u);
    if (unit_map[x][y].gfx == 1) sprintf(txt, "Dark Knight.   Moves Left: %d  (B): Stats",
                                             u);
    if (unit_map[x][y].gfx == 2) sprintf(txt, "Dark Archer.   Moves Left: %d  (B): Stats",
                                             u);
    if (unit_map[x][y].gfx == 3) sprintf(txt, "Dark Mage.     Moves Left: %d  (B): Stats",
                                             u);

    if (unit_map[x][y].gfx == 4) sprintf(txt, "Light Soldier. Moves Left: %d  (B): Stats",
                                             u);
    if (unit_map[x][y].gfx == 5) sprintf(txt, "Light Knight.  Moves Left: %d  (B): Stats",
                                             u);
    if (unit_map[x][y].gfx == 6) sprintf(txt, "Light Archer.  Moves Left: %d  (B): Stats",
                                             u);
    if (unit_map[x][y].gfx == 7) sprintf(txt, "Light Mage.    Moves Left: %d  (B): Stats",
                                             u);


    if (selected.x == -1)
    {
        rectfill(small_buffer, 0, small_buffer->h - 12, small_buffer->w, small_buffer->h, al2sdl_color(3) );
        //textout_ex(small_buffer, font, txt, 0, small_buffer->h - 8, 0, 3);
        textout_ex(small_buffer, 0, txt, 0, small_buffer->h - 12, 0, 3);
    }
    else
    {
        rectfill(small_buffer, 0, small_buffer->h - 12, small_buffer->w, small_buffer->h, al2sdl_color(2) );
        //textout_ex(small_buffer, font, txt, 0, small_buffer->h - 8, 0, 2);
        textout_ex(small_buffer, 0, txt, 0, small_buffer->h - 12, 0, 2);
    }

    //x = mouse_x / 16;
    //y = (mouse_y-14) / 16;
    //textprintf_ex( small_buffer, 0, 220,210, 0,3, "%d %d %d %d %d ", mouse_x, mouse_y, mouse_b, x,y );

}

void draw_turn_text(void)
{
    int w, h;

    w = 80;
    h = 40;

    show_message("");
    draw_screen();
    rest(150);

    change_music();

    rectfill(small_buffer, (small_buffer->w / 2) - w, (small_buffer->h / 2) - h,
             (small_buffer->w / 2) + w, (small_buffer->h / 2) + h, al2sdl_color(3) );

    rect(small_buffer, (small_buffer->w / 2) - w, (small_buffer->h / 2) - h,
         (small_buffer->w / 2) + w, (small_buffer->h / 2) + h, 0);

    if (game.side == 0)
    {
        //textprintf_centre_ex(small_buffer, font, small_buffer->w / 2, small_buffer->h * 0.4, 0, 3,"** The Dark **");
        textprintf_centre_ex(small_buffer, 0, small_buffer->w / 2, small_buffer->h * 0.4, 0, 3,"** The Dark **");
    }
    else
    {
        //textprintf_centre_ex(small_buffer, font, small_buffer->w / 2, small_buffer->h * 0.4, 0, 3,"-- The Light --");
        textprintf_centre_ex(small_buffer, 0, small_buffer->w / 2, small_buffer->h * 0.4, 0, 3,"-- The Light --");
    }

//textprintf_centre_ex(small_buffer, font, small_buffer->w / 2, small_buffer->h * 0.5, 0, 3, "Turn: %d", game.turn);
    textprintf_centre_ex(small_buffer, 0, small_buffer->w / 2, small_buffer->h * 0.5, 0, 3, "Turn: %d", game.turn);

//if (player[game.side].cpu == NO) textprintf_centre_ex(small_buffer, font, small_buffer->w / 2, small_buffer->h * 0.6, 0, 3, "Click Mouse Button");
    if (player[game.side].cpu == NO) textprintf_centre_ex(small_buffer, 0, small_buffer->w / 2, small_buffer->h * 0.6, 0, 3, "Press (A)");

    draw_screen();

    if (player[game.side].cpu == NO)
    {
        do
        {
            al2sdl_poll();
        }
        while (mouse_b != 0);

        do
        {
            al2sdl_poll();
        }
        while (mouse_b == 0);

        do
        {
            al2sdl_poll();
        }
        while (mouse_b != 0);
    }
    else
    {
        rest(750);
    }

    draw_map();
    draw_screen();
}

void draw_winner_text(void)
{
    int w, h;

    w = 80;
    h = 40;

    draw_screen();
    rest(350);

    rectfill(small_buffer, (small_buffer->w / 2) - w, (small_buffer->h / 2) - h,
             (small_buffer->w / 2) + w, (small_buffer->h / 2) + h, al2sdl_color(3) );

    rect(small_buffer, (small_buffer->w / 2) - w, (small_buffer->h / 2) - h,
         (small_buffer->w / 2) + w, (small_buffer->h / 2) + h, al2sdl_color(0));

    if (game.winning_side == 0)
    {
        textprintf_centre_ex(small_buffer, font, small_buffer->w / 2, small_buffer->h * 0.4, 0, 3,
                             "** The Dark **");
    }
    else
    {
        textprintf_centre_ex(small_buffer, font, small_buffer->w / 2, small_buffer->h * 0.4, 0, 3,
                             "-- The Light --");
    }

    textprintf_centre_ex(small_buffer, font, small_buffer->w / 2, small_buffer->h * 0.5, 0, 3,
                         "Are Victorious!");

    draw_screen();

    PlaySound( sfx_gameover );

    do
    {
        al2sdl_poll();
    }
    while (mouse_b != 0);

    do
    {
        al2sdl_poll();
    }
    while (mouse_b == 0);

    do
    {
        al2sdl_poll();
    }
    while (mouse_b != 0);
}







void draw_menu( int option )
{
    //clear_to_color( small_buffer, al2sdl_color(3) );
    blit( title_image, small_buffer, 0,0,0,0, 320,240 );
    //blit(tiles, small_buffer, 81, 0, 60, 20, 205, 64);

    blit( title_text, small_buffer, 0,0, 80,0, 160,72 );


    blit( title_options, small_buffer, 0,20*option, 80,210, 160,20 );

    //textout_centre_ex( small_buffer,0, "(B) to change, (A) to confirm", 160,230, 0,-1);
	textout_centre_ex( small_buffer,0, "Dpad to select, (A) to confirm", 160,230, 0,-1);
}

void draw_title_text()
{

    int done = false;

    int option = 0;

    draw_menu( option );
    draw_screen();

    while (!done)
    {
        al2sdl_poll();

        if ( key[KEY_ESC] )
        {
            player[0].cpu = -1;
            done = true;
        }

        if ( mouse_b == 1 )
        {
            player[0].cpu = NO;
            player[1].cpu = NO;
            done = true;
            switch ( option )
            {
            case 0:
                break; // Settings for this mode already set.
            case 1:
                player[1].cpu = YES;
                break;
            case 2:
                player[0].cpu = YES;
                break;
            case 3:
                player[0].cpu = YES;
                player[1].cpu = YES;
                break;
            case 4:
                draw_credits();
                option = 0;
                draw_menu( option );
                draw_screen();
                //SDL_Flip( screen );
                done = false;
                al2sdl_clear_input();
                break;
            case 5:
                player[0].cpu = -1;
                done = true;
                break;

            }
        }

        if ( mouse_b == 2 || dpad == 2 )
        {
            option += 1;
            if ( option > 5 ) option = 0;
            al2sdl_clear_input();
            PlaySound( sfx_chains );
            draw_menu(option);
            draw_screen();
        }
        else if ( dpad == 1 )
        {
            option -= 1;
            if ( option < 1 ) option = 5;
            al2sdl_clear_input();
            PlaySound( sfx_chains );
            draw_menu(option);
            draw_screen();
        }


    }// while(!done)

}





void draw_unit_info(int x, int y)
{
    int w, h;
    char txt[40];

    w = 80;
    h = 40;

    draw_screen();
    rest(150);

    rectfill(small_buffer, (small_buffer->w / 2) - w, (small_buffer->h / 2) - h,
             (small_buffer->w / 2) + w, (small_buffer->h / 2) + h, al2sdl_color(3));

    rect(small_buffer, (small_buffer->w / 2) - w, (small_buffer->h / 2) - h,
         (small_buffer->w / 2) + w, (small_buffer->h / 2) + h, al2sdl_color(0));

    if (unit_map[x][y].gfx == 0) sprintf(txt, "Dark Soldier");
    if (unit_map[x][y].gfx == 1) sprintf(txt, "Dark Knight");
    if (unit_map[x][y].gfx == 2) sprintf(txt, "Dark Archer");
    if (unit_map[x][y].gfx == 3) sprintf(txt, "Dark Mage");

    if (unit_map[x][y].gfx == 4) sprintf(txt, "Light Soldier");
    if (unit_map[x][y].gfx == 5) sprintf(txt, "Light Knight");
    if (unit_map[x][y].gfx == 6) sprintf(txt, "Light Archer");
    if (unit_map[x][y].gfx == 7) sprintf(txt, "Light Mage");

    textprintf_centre_ex(small_buffer, font, small_buffer->w / 2, small_buffer->h * 0.35, 0, 3,
                         txt);

    textprintf_centre_ex(small_buffer, font, small_buffer->w / 2, small_buffer->h * 0.45, 0, 3,
                         "Attack: %d", unit_map[x][y].attack);
    textprintf_centre_ex(small_buffer, font, small_buffer->w / 2, small_buffer->h * 0.5, 0, 3,
                         "Defence: %d", unit_map[x][y].defence);
    textprintf_centre_ex(small_buffer, font, small_buffer->w / 2, small_buffer->h * 0.55, 0, 3,
                         "Movement: %d", unit_map[x][y].movement);
    textprintf_centre_ex(small_buffer, font, small_buffer->w / 2, small_buffer->h * 0.6, 0, 3,
                         "Attack Range: %d", unit_map[x][y].attack_range);

    draw_screen();

    if (player[game.side].cpu == NO)
    {
        do
        {
            al2sdl_poll();
        }
        while (mouse_b != 0);

        do
        {
            al2sdl_poll();
        }
        while (mouse_b == 0);

        do
        {
            al2sdl_poll();
        }
        while (mouse_b != 0);
    }
    else
    {
        rest(750);
    }

    draw_map();
    draw_screen();
}


SDL_Surface *cursor_bg;
void redraw_cursor_bg()
{
    // Restore background
    if ( cursor_bg ) blit( cursor_bg, small_buffer, 0,0, ocurx * 16 - 1, ocury * 16 + 14 - 1, 18,18 );
}

void draw_cursor()
{
    if ( player[game.side].cpu == YES ) return;

    if (!cursor_bg)
    {
        cursor_bg = SDL_CreateRGBSurface( surface_type, 18,18,16, 0,0,0,0 );
    }

    // Save the background
    blit( small_buffer, cursor_bg, curx * 16 - 1, cury * 16 + 14 - 1, 0,0, 18,18 );

    // Draw cursor
    //rect( small_buffer, curx * 16, cury * 16 + 14, curx * 16+15, cury * 16 + 14 +15, al2sdl_color(0) );
    //rect( small_buffer, curx * 16+1, cury * 16 + 15, curx * 16+14, cury * 16 + 14 +14, al2sdl_color(1) );
    if ( selected.x == -1 || unit_map[selected.x][selected.y].movement <= 0 )
        blit( cursor, small_buffer, 0,0, curx * 16 - 1, cury * 16 + 14 - 1, 18,18 );
    else
        blit( cmove, small_buffer, 0,0, curx * 16 - 1, cury * 16 + 14 - 1, 18,18 );


}



void draw_credits()
{

    clear_to_color( small_buffer, SDL_MapRGB( small_buffer->format, 255,255,255 ) );

    blit(tiles, small_buffer, 81, 0, 60, 20, 205, 64);

    textout_centre_ex( small_buffer,0, "By Richard Phipps", 160,120, 0,3 );
    textout_centre_ex( small_buffer,0, "SDL and GP2X version by Peter Sundling", 160,135, 0,3 );
    textout_centre_ex( small_buffer,0, "Music by chaozz.nl", 160,150, 0,3 );

    textout_centre_ex( small_buffer,0, "Press (A) to continue", 160,200, 0,3 );

    draw_screen();

    al2sdl_clear_input();
    while (mouse_b != 1 )
    {
        al2sdl_poll();

    }

}


void show_message( char *fmt, ... )
{
    //strcpy( message, msg );
    va_list ptr;

    va_start(ptr, fmt);
    vsprintf(message, fmt, ptr);

    //textout_ex( dest, 0, buff, x,y, color, bg );

    //free( buff );
    va_end(ptr);
    draw_map();
}
