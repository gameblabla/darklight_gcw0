/*

    TODO: Screen flickers randomly on the gp2x
    TODO: Remove use of al2sdl using threads/timer


*/


#include <SDL/SDL.h>
#include <SDL/SDL_image.h>



#include <time.h>

#include "DarkLight.h"
#include "unit.h"
#include "log.h"
#include "ai.h"
#include "al2sdl.h"
#include "screen.h"

const int SCREEN_WIDTH  = 320;
const int SCREEN_HEIGHT = 240;

int surface_type = SDL_HWSURFACE;

SDL_Surface *screen = NULL;

SDL_Surface *title_image = NULL;
SDL_Surface *title_options = NULL;
SDL_Surface *tiles = NULL;
SDL_Surface *title_text = NULL;

SDL_Surface *balance = NULL;
SDL_Surface *balancebg = NULL;

SDL_Surface *cursor = NULL;
SDL_Surface *cmove = NULL;

SDL_Surface *small_buffer = NULL;
//SDL_Surface *large_buffer;

SDL_Surface *map = NULL;
SDL_Surface *army = NULL;

TTF_Font* sdlfont = NULL;

Mix_Chunk *sfx_hit = NULL;
Mix_Chunk *sfx_arrow = NULL;
Mix_Chunk *sfx_fireball = NULL;
Mix_Chunk *sfx_spawn = NULL;
Mix_Chunk *sfx_castle = NULL;
Mix_Chunk *sfx_step = NULL;

Mix_Chunk *sfx_chains = NULL;
Mix_Chunk *sfx_gameover = NULL;

Mix_Music *mus_title = NULL;
Mix_Music *mus_balanced = NULL;
Mix_Music *mus_light = NULL;
Mix_Music *mus_dark = NULL;

s_game game;
s_player player[MAX_PLAYERS];

int curx, cury, ocurx, ocury;


int main( int argc, char* args[] )
{

    startup_game();


    if ( Mix_PlayMusic( mus_title, -1 ) == -1 )
        record_error("Unable to play music: %s", Mix_GetError());

    ChangeVolume(0);

    int quit_main_menu = false;
    while ( !quit_main_menu )
    {
        draw_title_text();

        if (player[0].cpu == -1)
            quit_main_menu = true;
        else
        {
            Mix_HaltMusic();
            main_loop();
        }

        al2sdl_poll();
        al2sdl_clear_input();
        if ( Mix_PlayMusic( mus_title, -1 ) == -1 )
            record_error("Unable to play music: %s", Mix_GetError());
    }

    shutdown_game();


    // Do GP2X specific stuff
#ifdef PLATFORM_GP2X
    // Return to the menu
    chdir("/usr/gp2x");
    execl("/usr/gp2x/gp2xmenu", "/usr/gp2x/gp2xmenu", NULL);
#endif // PLATFORM_GP2X

    return 0;
}



void startup_game()
{
    open_log("logfile.txt");
    write_log_header();

    write_to_log("Randomize");
    srand(time(0));

    write_to_log("Initiating SDL");
    // Initialize all SDL subsystems
    if ( SDL_Init( SDL_INIT_VIDEO ) == -1 )
    {
        record_error("Unable to init SDL.");
    }

    // Create the screen surface
	SDL_ShowCursor( SDL_DISABLE );
    #ifdef RS97
    screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, 16, SDL_HWSURFACE);  
    #else
    screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, 16, surface_type | SDL_DOUBLEBUF );
    #endif

    write_to_log("Videomode set");

    // Do PC specific stuff
#ifdef PLATFORM_PC
    // Set the window caption
    SDL_WM_SetCaption( "Dark Light Battles", NULL );
#endif // PLATFORM_PC

    // Update the screen
    if ( SDL_Flip( screen ) == -1 )
    {
        record_error("Unable to flip the screen.");
    }

    write_to_log("Loading font");
    TTF_Init();
    sdlfont = TTF_OpenFont("Data/arcadepi.ttf", 10);
    if (!sdlfont)
    {
        record_error("Unable to load font!");
    }

    // Show loading message
    textout_centre_ex( screen, 0, "Loading", 160,110, al2sdl_color(3), -1 );
    textout_centre_ex( screen, 0, "This might take a while...", 160,140, al2sdl_color(3), -1 );
    SDL_Flip( screen );

    write_to_log("Starting up mixer");
    int audio_rate = 11025;
    Uint16 audio_format = AUDIO_S16SYS;
    int audio_channels = 2;
    int audio_buffers = 128;

    if (Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) != 0)
    {
        record_error( "Unable to initialize audio: %s\n", Mix_GetError());
    }

    write_to_log("Creating buffers");

    //

    small_buffer = SDL_CreateRGBSurface(surface_type,320,240,16,0,0,0,0);
    //small_buffer = screen;
    //large_buffer = SDL_CreateRGBSurface(surface_type,640,480,16,0,0,0,0);
    map = SDL_CreateRGBSurface(SDL_SWSURFACE,TILES_X,TILES_Y,16,0,0,0,0);
    //clear_to_color(map, 0);

    SDL_Surface *tmp = IMG_Load("Data/tiles.bmp");
    if (!tmp)
    {
        record_error("Unable to load tiles!");
    }
    tiles = SDL_ConvertSurface( tmp, screen->format, surface_type );
    SDL_SetColorKey( tiles, SDL_SRCCOLORKEY, SDL_MapRGB( screen->format, 174,105,174) );

    tmp = IMG_Load("Data/title.png");
    if (!tmp)
    {
        record_error("Unable to load title!");
    }
    title_image = SDL_ConvertSurface( tmp, screen->format, surface_type );

    tmp = IMG_Load("Data/title_text.png");
    if (!tmp)
    {
        record_error("Unable to load title text!");
    }
    title_text = SDL_ConvertSurface( tmp, screen->format, surface_type );
    SDL_SetColorKey( title_text, SDL_SRCCOLORKEY, SDL_MapRGB( screen->format, 174,105,174) );


    tmp = IMG_Load("Data/options.png");
    if (!tmp)
    {
        record_error("Unable to load title options!");
    }
    title_options = SDL_ConvertSurface( tmp, screen->format, surface_type );
    SDL_SetColorKey( title_options, SDL_SRCCOLORKEY, SDL_MapRGB( screen->format, 174,105,174) );


    tmp = IMG_Load("Data/balance.png");
    if (!tmp)
    {
        record_error("Unable to balance image!");
    }
    balance = SDL_ConvertSurface( tmp, screen->format, surface_type );
    //SDL_SetColorKey( balance, SDL_SRCCOLORKEY, SDL_MapRGB( screen->format, 174,105,174) );


    tmp = IMG_Load("Data/balancebg.png");
    if (!tmp)
    {
        record_error("Unable to balance background!");
    }
    balancebg = SDL_ConvertSurface( tmp, screen->format, surface_type );

    tmp = IMG_Load("Data/cursor.png");
    if (!tmp)
    {
        record_error("Unable to load cursor!");
    }
    cursor = SDL_ConvertSurface( tmp, screen->format, surface_type );
    SDL_SetColorKey( cursor, SDL_SRCCOLORKEY, SDL_MapRGB( screen->format, 174,105,174) );

    tmp = IMG_Load("Data/cmove.png");
    if (!tmp)
    {
        record_error("Unable to load move cursor!");
    }
    cmove = SDL_ConvertSurface( tmp, screen->format, surface_type );
    SDL_SetColorKey( cmove, SDL_SRCCOLORKEY, SDL_MapRGB( screen->format, 174,105,174) );



    write_to_log("Loading sound effects and music");

    sfx_hit = Mix_LoadWAV("Data/hit.wav");
    if ( !sfx_hit ) record_error("Unable to load hit.wav");

    sfx_arrow = Mix_LoadWAV("Data/arrow.wav");
    if ( !sfx_arrow ) record_error("Unable to load arrow.wav");

    sfx_fireball = Mix_LoadWAV("Data/fireball.wav");
    if ( !sfx_fireball ) record_error("Unable to load fireball.wav");

    sfx_spawn = Mix_LoadWAV("Data/spawn.wav");
    if ( !sfx_spawn ) record_error("Unable to load spawn.wav");

    sfx_castle = Mix_LoadWAV("Data/castle.wav");
    if ( !sfx_castle ) record_error("Unable to load castle.wav");

    sfx_step = Mix_LoadWAV("Data/step.wav");
    if ( !sfx_step ) record_error("Unable to load step.wav");

    sfx_chains = Mix_LoadWAV("Data/chains.wav");
    if ( !sfx_chains ) record_error("Unable to load chains.wav");

    sfx_gameover = Mix_LoadWAV("Data/gameover.wav");
    if ( !sfx_gameover ) record_error("Unable to load gameover.wav");


    mus_title = Mix_LoadMUS("Data/creek.ogg");
    if ( !mus_title ) record_error("Unable to open the title music!");

    mus_balanced = Mix_LoadMUS("Data/pyramid.mod");
    if ( !mus_balanced ) record_error("Unable to open the balanced music!");

    mus_light = Mix_LoadMUS("Data/spacemaz.mod");
    if ( !mus_light ) record_error("Unable to open the light music!");

    mus_dark = Mix_LoadMUS("Data/fastelmo.ogg");
    if ( !mus_dark ) record_error("Unable to open the dark music!");

    init_al2sdl();


    write_to_log("Setting up units...");
    setup_units();

    write_to_log("startup_game done");
}




void shutdown_game(void)
{
    write_to_log("shutdown_game called");

    write_to_log("Shutting down SDL_Ttf");
    if ( sdlfont ) TTF_CloseFont(sdlfont);	// Release the memory allocated to font
    TTF_Quit();						// Close SDL_TTF

    write_to_log("Releasing surfaces");
    if ( tiles ) SDL_FreeSurface( tiles );
    if ( small_buffer ) SDL_FreeSurface( small_buffer );
    //SDL_FreeSurface( large_buffer );
    if ( title_image ) SDL_FreeSurface( title_image );
    if ( title_options ) SDL_FreeSurface( title_options );
    if ( map ) SDL_FreeSurface( map );
    if ( army ) SDL_FreeSurface( army );

    if( cursor ) SDL_FreeSurface( cursor );
    if( cmove ) SDL_FreeSurface( cmove );

    if ( balance ) SDL_FreeSurface( balance );
    if ( balancebg ) SDL_FreeSurface( balancebg );

    write_to_log("Releasing sound effects and music");
    Mix_HaltMusic();

    if ( sfx_hit ) Mix_FreeChunk( sfx_hit );
    if ( sfx_arrow ) Mix_FreeChunk( sfx_arrow );
    if ( sfx_fireball ) Mix_FreeChunk( sfx_fireball );
    if ( sfx_spawn ) Mix_FreeChunk( sfx_spawn );
    if ( sfx_castle ) Mix_FreeChunk( sfx_castle );
    if ( sfx_step ) Mix_FreeChunk( sfx_step );
    if ( sfx_chains ) Mix_FreeChunk( sfx_chains );
    if ( sfx_gameover ) Mix_FreeChunk( sfx_gameover );

    if ( mus_title ) Mix_FreeMusic( mus_title );
    if ( mus_balanced ) Mix_FreeMusic( mus_balanced );
    if ( mus_light ) Mix_FreeMusic( mus_light );
    if ( mus_dark ) Mix_FreeMusic( mus_dark );

    Mix_CloseAudio();

    write_to_log("Shutting down SDL");
    SDL_Quit();


    write_to_log("Shutting down logfile and exiting game.");
    close_log();
}




void main_loop(void)
{
    int done, turn_done;

    write_to_log("New game started.");
    write_to_log("P1 is cpu: %s", player[0].cpu == YES?"yes":"no");
    write_to_log("P2 is cpu: %s", player[1].cpu == YES?"yes":"no");

    Mix_PlayMusic( mus_balanced, -1 );
    music_balance = 0;
    last_music_change = SDL_GetTicks();
    ChangeVolume(0);

    show_message("Welcome to the battle!");
    game.side = 0;

//int x, y;
    done = NO;
    selected.x = -1;
    game.turn = 1;
    game.winning_side = -1;

    curx = TILES_X / 2;
    cury = TILES_Y / 2;

    setup_unit_map();
    create_map();

    clear_to_color( small_buffer, al2sdl_color(0) );

    draw_map();

start:
    game.units_who_can_move = count_units_who_can_move();
    draw_turn_text();
    turn_done = NO;


    do
    {
        ocurx = curx;
        ocury = cury;
        al2sdl_poll();


        if (game.units_who_can_move < 1) turn_done = YES;
        if (game.winning_side != -1) done = YES;
        if (player[game.side].cpu == YES)
        {
            do_ai_unit_sweep();
            turn_done = YES;
        }

        //x = mouse_x / 16;
        //y = (mouse_y-14) / 16;
        draw_status_text(curx, cury);

        if (mouse_b == 1 && player[game.side].cpu == NO )
        {
            //redraw_cursor_bg();
            if (selected.x == -1)
            {
                select_unit(curx, cury);
            }
            else
            {
                try_to_move_unit(curx, cury);
                al2sdl_clear_input();
            }
        }

        if (mouse_b == 2 && player[game.side].cpu == NO )
        {
            //redraw_cursor_bg();
            if (selected.x != -1)
            {
                unit_map[selected.x][selected.y].movement = -1;
                unselect_unit();
                game.units_who_can_move--;
            }
            else
            {
                if (unit_map[curx][cury].gfx != -1) draw_unit_info(curx, cury);
            }
        }

        if (mouse_b == 3 && player[game.side].cpu == NO )
        {
            if (selected.x == -1) turn_done = YES;
        }

        if ( ocurx != curx || ocury != cury )
        {
            //redraw_cursor_bg();
            //draw_cursor();
            draw_map();
            if ( selected.x != -1 )
                highlight_enemy_units();
        }

        draw_screen();

        if (key[KEY_ESC]) done = YES;
    }
    while (done == NO && turn_done == NO);

    if (done == NO)
    {
        // End of Turn..
        if (game.side == 0)
        {
            game.side = 1;
            goto start;
        }
        else
        {
            game.side = 0;
            game.turn++;
            restore_all_units_movement();

            goto start;
        }
    }
    else
    {
        if (game.winning_side != -1) draw_winner_text();
    }
}


