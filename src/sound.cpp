
#include "DarkLight.h"
#include "sound.h"
#include "screen.h"
#include "unit.h"
#include "log.h"

int sound_volume = 64;
int music_balance = 0;
int last_music_change = 0;

int PlaySound( Mix_Chunk *snd )
{
    int ch = Mix_PlayChannel( -1, snd, 0 );
    Mix_Volume(ch, sound_volume );
    return ch;
}

void PlayMusic( Mix_Music *mus )
{
    Mix_PlayMusic( mus, -1 );
    Mix_VolumeMusic( sound_volume );
}

void StopSound()
{
    Mix_HaltChannel(-1);
}

void StopMusic()
{
    Mix_HaltMusic();
}

void StopAll()
{
    StopSound();
    StopMusic();
}

void ChangeVolume( int offset )
{
    sound_volume += offset;
    if ( sound_volume < 0 ) sound_volume = 0;
    if ( sound_volume > MIX_MAX_VOLUME ) sound_volume = MIX_MAX_VOLUME;

    Mix_Volume( -1, sound_volume );
    Mix_VolumeMusic( sound_volume );

}


void change_music()
{
    int x,y;
    int dsize = 0, lsize = 0;

    if ( SDL_GetTicks() < last_music_change+30000 )
        return;

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
    float winning = ((float)dsize / (dsize+lsize));

    //write_to_log("Changing music...");
    //write_to_log("   Winning: %.2f", winning);
    //write_to_log("   Old music_balance: %d", music_balance);

    if ( winning < 0.30 && music_balance != -1 ) // Light is winning
    {
        music_balance = -1;
        Mix_PlayMusic( mus_light, -1 );
        last_music_change = SDL_GetTicks();
        //write_to_log("   Changed to light");
    }
    else if ( winning > 0.70 && music_balance != 1 ) // Dark is winning
    {
        music_balance = 1;
        Mix_PlayMusic( mus_dark, -1 );
        last_music_change = SDL_GetTicks();
        //write_to_log("   Changed to dark");
    }
    /*
    else if ( winning >= 0.3 && winning <= 0.7 && music_balance != 0 )
    {
        music_balance = 0;
        Mix_PlayMusic( mus_balanced, -1 );
        last_music_change = SDL_GetTicks();
        //write_to_log("   Changed to balanced");
    }
    */

    //write_to_log("   New music_balance: %d", music_balance);

    ChangeVolume( 0 );
}

