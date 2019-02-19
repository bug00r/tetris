#ifndef TETRIS_APP_H
#define TETRIS_APP_H

#include <time.h>
#include <stdbool.h>
#include <stdlib.h>

#include "SDL2/SDL.h"

#include "tetris_game.h"
#include "tetris_input.h"
#include "tetris_ui.h"
#include "tetris_sfx.h"

typedef struct {
	bool done;
} tetris_app_state_t;

typedef struct {
	tetris_app_state_t 	state;	
	tetris_ui_t 		ui;
	tetris_game_t 		game;
	tetris_input_t 		input;
	tetris_sfx_t 		sfx;		
	int argc; 
	char **argv;
} tetris_app_t;

bool tetris_app_init(tetris_app_t *app, int argc, char **argv);  
void tetris_app_run(tetris_app_t *app);
void tetris_app_cleanup(tetris_app_t *app);

#endif