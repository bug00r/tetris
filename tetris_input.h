#ifndef TETRIS_INPUT_H
#define TETRIS_INPUT_H

#include <stdlib.h>
#include <stdbool.h>

#include "SDL2/SDL.h"

typedef struct {
	Uint32 type;
	SDL_Keycode keycode;
	Uint8 repeat;
} tetris_input_state_t;

typedef struct {
	tetris_input_state_t state;
	SDL_TimerID timer;
} tetris_input_t;

bool tetris_input_init(tetris_input_t *input);

Uint32 tetris_input_update_down(Uint32 interval, void *param);

void tetris_input_update(tetris_input_t *input);

#endif