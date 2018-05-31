
#ifndef SOUND_H_
#define SOUND_H_

#include <SDL/SDL_mixer.h>

int PlaySound( Mix_Chunk *snd );
void PlayMusic( Mix_Music *mus );
void StopSound();
void StopMusic();
void StopAll();

void ChangeVolume( int offset );

void change_music();

#endif
