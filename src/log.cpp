// Log (.C)
// --------

//#include <allegro.h>
#include <stdio.h>
#include <stdarg.h>
#include "log.h"
#include "DarkLight.h"
#include "al2sdl.h"

// Logging and error trapping functions:
// -------------------------------------

FILE *logfile;

int open_log(char *filename)
{
    /*if ((logfile = fopen(filename, "w")) != NULL) return 0;

    return -1;*/
    return 0;
}

int write_to_log(char *format, ...)
{
    //va_list ptr; 
    int status = -1;

    /*if (logfile != NULL)
    {
        va_start(ptr, format);

        //Write to logfile.
        status = vfprintf(logfile, format, ptr); // Write passed text.
        fprintf(logfile, "\n"); // New line..

        va_end(ptr);

        //Flush file.
        fflush(logfile);
    }*/

    return status;
}

int write_to_log_no_cr(char *format, ...)
{
    //va_list ptr;
    int status = -1;

    /*if (logfile != NULL)
    {
        va_start(ptr, format);
        status = vfprintf(logfile, format, ptr); // Write passed text.

#ifndef PLATFORM_GP2X
        vfprintf(stdout, format, ptr);
#endif

        va_end(ptr);
        fflush(logfile);
    }*/

    return status;
}

int record_error(char *format, ...)
{
	//va_list ptr;
    int status = -1;
/*
    if (logfile != NULL)
    {
        // initialize ptr to point to the first argument after the format string
        va_start(ptr, format);

        // Write to logfile.
        status = vfprintf(logfile, format, ptr); // Write passed text.
        fprintf(logfile, "\n"); // New line..

        va_end(ptr);

        // Flush file.
        fflush(logfile);

        //play_sound(default_sound.error, 255, 128, 1000, 1);
        write_to_log("--controlled exit due to error--");

#ifdef PLATFORM_GP2X
        if ( screen )
        {
            for ( int i = 0; i < 5; i++ )
            {
                if ( sdlfont )
                {
                    SDL_Rect r;
                    r.x = 0;
                    r.y = 0;
                    r.w = 320;
                    r.h = 240;
                    SDL_FillRect( screen, &r, SDL_MapRGB( screen->format, 0,0,0 ) );
                    textout_ex( screen, 0, "CRITICAL ERROR. CHECK LOGFILE!", 0,0, SDL_MapRGB( screen->format, 255,0,0 ),-1 );
                    SDL_Flip( screen );
                }
                else
                {
                    // We don't have a font... Fill screen with red to signal an error
                    SDL_Rect r;
                    r.x = 0;
                    r.y = 0;
                    r.w = 320;
                    r.h = 240;
                    SDL_FillRect( screen, &r, SDL_MapRGB( screen->format, 255,0,0 ) );
                    SDL_Flip( screen );
                }
            }
        }
#endif

    }
*/
    shutdown_game();
    exit(0);

    return status;
}


int close_log(void)
{
    int status = -1;
/*
    if (logfile != NULL)
    {
        fflush(logfile);
        fclose(logfile);
    }
*/
    return status;
}

// Support functions.

int write_log_header(void)
{
// Write the log header file with the details of the game name, author etc.
// These are all defines listed in "game.h"
/*
    write_to_log(GAME_NAME);
    write_to_log("----------------------------");
    write_to_log("Version : %s, Started : %s", GAME_VERSION, GAME_DATE);
    write_to_log("Author : %s", GAME_AUTHOR);
    write_to_log("----------------------------");
    write_to_log("");
*/
    return YES;
}

// Used with our error trapping code.
int overrun_error(char *struc, int pos, int line, char *file)
{
// Example overrun_error function.
 /*   char text[120];

//text_mode(0);
    sprintf(text, "Bounds Overrun: %s[%d]", struc, pos);
//textout_centre(screen, font, text, SCREEN_W/2, SCREEN_H/2, makecol(255, 255, 255));
    write_to_log(text);

    sprintf(text, "(line: %d of %s)", line, file);
//textout_centre(screen, font, text, SCREEN_W/2, (SCREEN_H/2) + 8, makecol(255, 255, 255));
    write_to_log(text);

*/
    shutdown_game();
}
