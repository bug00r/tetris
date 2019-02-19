#ifndef TETRIS_GAME_H
#define TETRIS_GAME_H

#include <time.h>
#include <stdlib.h>
#include <stdbool.h>

#include "SDL2/SDL.h"

#include "tetris.h"
#include "tetris_input.h"

typedef struct {
	bool quit;
	bool game_over;
	bool new_game;
	bool was_paused;
	bool was_started;
	bool is_running;
	bool stone_was_moved;
	bool stone_was_rotated;
	bool stone_was_landed;
	bool change_texture;
	bool got_full_lines;
	bool removing_full_lines;
	bool got_level_up;
	stone_type_t last_set_stone;
	clock_t begin_removing;
	tetris_full_lines_t full_lines;
} tetris_game_state_t;

typedef struct {
	tetris_game_state_t last_state;
	tetris_game_state_t state;
	tetris_t 			*tetris;
	unsigned int 		score;		
	unsigned int 		level;
	unsigned int 		lines;
	unsigned int 		update_intervall; 
	stone_type_t 		next_stone;
	current_stone_t		last_stone;
} tetris_game_t;

bool tetris_game_init(tetris_game_t *game);
void tetris_game_cleanup(tetris_game_t *game);

void tetris_game_update(tetris_game_t *game, tetris_input_t *input, clock_t *ticks);

#endif