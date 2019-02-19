#ifndef TETRIS_SFX_H
#define TETRIS_SFX_H

#include <stdlib.h>
#include <stdbool.h>

#include "SDL2/SDL.h"
#include "SDL2/SDL_mixer.h"

#include "tetris_defs.h"
#include "tetris_game.h"

typedef struct {
	Mix_Music *sfx;
	Mix_Chunk *sfx2;
	Mix_Chunk *sfx3; 
	Mix_Chunk *sfx4;
} tetris_sfx_t;

bool init_tetris_sfx(tetris_sfx_t *sfx);

void tetris_sfx_update(tetris_sfx_t *sfx, tetris_game_t *game);

#endif