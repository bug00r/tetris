#include "tetris_game.h"

static void __tetris_game_reset_stats(tetris_game_t *game) {
	game->score = 0;
	game->level = 1;
	game->lines = 0;
	game->update_intervall = 850;
	game->next_stone = TETRIS_NO_STONE;
}

static void __tetris_game_state(tetris_game_t *game) {
	tetris_t *tetris = game->tetris;
	tetris_game_state_t *state = &game->state;
	state->was_paused = tetris_was_paused(tetris);
	state->was_started = tetris_was_started(tetris);
	state->is_running = tetris_is_running(tetris);
	state->stone_was_moved = false;
	state->stone_was_rotated = false;
	state->quit = false;
	state->last_set_stone = TETRIS_NO_STONE;
	state->stone_was_landed = false;
	state->game_over = false;
	state->new_game = false;
	state->change_texture = false;
	state->removing_full_lines = false;
	state->begin_removing = 0;
}

bool tetris_game_init(tetris_game_t *game) {
	bool init_success = false;
	
	if (game != NULL) {
		game->tetris = tetris_new();
		__tetris_game_reset_stats(game);
		__tetris_game_state(game);
		init_success = true;
	}
	
	return init_success;
}

void tetris_game_cleanup(tetris_game_t *game) {
	tetris_free(&game->tetris);
}

static void __tetris_create_new_stone(tetris_game_t * game, tetris_create_stone_response_t *stone_response) {
	
	bool is_new_stone = false;
	if ( game->next_stone == TETRIS_NO_STONE ) {
		game->next_stone = tetris_random_stone();
		is_new_stone = true;
	}
	
	if (!is_new_stone) {
		game->last_stone = game->tetris->active_stone;
	}
	
	tetris_create_stone(game->next_stone, game->tetris, stone_response);
	
	if (is_new_stone) {
		game->last_stone = game->tetris->active_stone;
	}
	
	game->next_stone = tetris_random_stone();
}

static void __start_new_game(tetris_game_t * game, tetris_input_t *input, tetris_create_stone_response_t *stone_response) {
	#if debug
		SDL_Log("GAME: start new game\n");
	#endif
	if ( input->timer != 0 ) {
		#if debug
		SDL_Log("GAME: delete old move down timer\n");
		#endif
		SDL_RemoveTimer(input->timer);
		input->timer = 0;
	}
	
	if (game->tetris != NULL) {
		tetris_free(&game->tetris);
	}
	game->tetris = tetris_new();
	tetris_start(game->tetris);

	__tetris_game_reset_stats(game);
	
	__tetris_create_new_stone(game, stone_response);
	
	game->state.new_game = true;
	game->state.game_over = false;

	#if debug
		SDL_Log("GAME: start new timer\n");
	#endif
	input->timer = SDL_AddTimer(game->update_intervall, tetris_input_update_down, (void*)&game->update_intervall);
}

static void game_update_level_score_lines(tetris_game_t *game, tetris_move_response_t *response, stone_type_t *type) {
	tetris_game_state_t *state = &game->state;
	/* redundant points code */ 
	if (response->full_lines != NULL) {
		state->got_full_lines = true;
		state->removing_full_lines = true;
		state->full_lines = *response->full_lines;
		game->lines += response->full_lines->cnt;
		game->score += response->full_lines->cnt * 150 * game->level;
		if ( (game->lines >= (game->level * 10)) ) {
			state->got_level_up = true;
			game->level++;
			if ( game->update_intervall > 150 ) {
				game->update_intervall -= 70;
			} else if ( game->update_intervall > 50) {
				game->update_intervall -= 10;
			} else if (game->update_intervall <= 10) {
				game->update_intervall -= 1;
			} else if (game->update_intervall <= 1) {
				game->update_intervall = 1;
			}
		}
	}
	game->score += (*type * game->level);
}

static void __move_stone_down(tetris_game_t * game, tetris_move_response_t *response, 
							tetris_create_stone_response_t *stone_response) {
							
	tetris_game_state_t *last_state = &game->last_state;
	tetris_game_state_t *state = &game->state;
	#if debug
			SDL_Log("GAME: Try to move down. pause: %i, running: %i\n", last_state->was_paused, last_state->is_running);
	#endif
	if ( !last_state->was_paused && last_state->is_running ) {
		tetris_t *tetris = game->tetris;
		#if debug
			SDL_Log("GAME: moving down\n");
		#endif
		game->last_stone = game->tetris->active_stone;
		tetris_move_stone_down(tetris, response);
		
		if ( !response->was_moved ) {
		
			state->last_set_stone = tetris->active_stone.type;
			state->stone_was_landed = true;

			game_update_level_score_lines(game, response, &game->last_stone.type);
			
			__tetris_create_new_stone(game, stone_response);

		} else {
			state->last_set_stone = TETRIS_NO_STONE;
			state->stone_was_landed = false;
		}
	}
}

static void __game_check_game_over(tetris_game_t *game, tetris_input_t *input, tetris_create_stone_response_t *stone_response) {
	/* game over */
	if (stone_response->type == TETRIS_MAX_STONE) {
		game->state.game_over = true;

		tetris_stop(game->tetris);
		if ( input->timer != 0 ) {
			SDL_RemoveTimer(input->timer);
			input->timer = 0;
		}
	}
}

static bool __game_check_removing_lines(tetris_game_t *game, clock_t *ticks) {
	bool still_removing_lines = false;
	if ( game->state.removing_full_lines  ) {
		if (game->state.begin_removing == 0) {
			game->state.begin_removing = *ticks;
		}
		if ( (*ticks - game->state.begin_removing) >= 1000 ) {
			game->state.removing_full_lines = false;
			game->state.begin_removing = 0;
		} else {
			still_removing_lines = true;
		}
	}
	return still_removing_lines;
}

void tetris_game_update(tetris_game_t *game, tetris_input_t *input, clock_t *ticks) {
	
	/* save game current state int last_state */
	game->last_state = game->state;
	
	if (__game_check_removing_lines(game, ticks)) return;
		
	tetris_game_state_t *last_state = &game->last_state;
	tetris_game_state_t *state = &game->state;
	/* update current state based on input */
	/* faster move down */
	Uint32 type = input->state.type;
	SDL_Keycode keycode = input->state.keycode;
	
	tetris_t *tetris = game->tetris;
	
	tetris_move_response_t response;
	response.full_lines = NULL;
	tetris_reset_move_response(tetris, &response);	
	
	tetris_create_stone_response_t stone_response;
	
	/* reset some state */
	/* if game was created before, set to false */
	state->new_game = false;
	state->got_full_lines = false;
	state->got_level_up = false;
	state->change_texture = false;
	state->stone_was_moved = false;
	state->stone_was_rotated = false;
	state->stone_was_landed = false;
	state->change_texture = false;
	
	/* check current game states */
	state->was_paused = tetris_was_paused(tetris);
	state->was_started = tetris_was_started(tetris);
	state->is_running = tetris_is_running(tetris);
	
	
	if ( ( type == SDL_KEYDOWN && keycode == SDLK_DOWN ) || type == SDL_USEREVENT) {
		#if debug
			SDL_Log("INPUT: move down triggered by keyboard: %i  by update: %i \n", 
												( type == SDL_KEYDOWN && keycode == SDLK_DOWN),
												( type == SDL_USEREVENT));
		#endif
		__move_stone_down(game, &response, &stone_response);
	} else if (type == SDL_KEYDOWN) {
		switch(keycode) {
			/* quit application */
			case SDLK_ESCAPE: 	state->quit = true; break;	
			/* start new game */
			case SDLK_F1:		__start_new_game(game, input, &stone_response);
								break;
			/* pause game */
			case SDLK_F4: 		if ( !last_state->was_paused && last_state->is_running ) {
									tetris_pause(tetris);
									last_state->was_paused = true;
								} else if ( last_state->was_paused ) {
									tetris_continue(tetris);
									last_state->was_paused = false;
								}
								
								break;
			case SDLK_F5: 		if ( !last_state->was_paused && last_state->is_running ) {
									state->change_texture = true;
								}
								break;
			/* move current stone left */		
			case SDLK_LEFT:		if ( !last_state->was_paused && last_state->is_running ) {
									game->last_stone = game->tetris->active_stone;
									tetris_move_stone_left(tetris, &response);
								}
								break;					
			/* move current stone right */
			case SDLK_RIGHT:	if ( !last_state->was_paused && last_state->is_running ) {
									game->last_stone = game->tetris->active_stone;
									tetris_move_stone_right(tetris, &response);
								}
								break;					
			case SDLK_UP:		if ( !last_state->was_paused && last_state->is_running ) {
									game->last_stone = game->tetris->active_stone;
									tetris_rotate_stone(tetris, &response);
								}
								break;					
			
		}
	}
	
	game->state.stone_was_moved = response.was_moved;
	game->state.stone_was_rotated = response.was_rotated;
	
	__game_check_game_over(game, input, &stone_response);
	
}