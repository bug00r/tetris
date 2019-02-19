#include "tetris_input.h"

bool tetris_input_init(tetris_input_t *input) {
	bool init_success = false;
	if (input != NULL) {
		input->timer = 0;
		init_success = true;
	}
	return init_success;
}

Uint32 tetris_input_update_down(Uint32 interval, void *param)
{
	#if debug
		SDL_Log("INPUT: move down callback, intervall: %i\n", interval);
	#endif
 
	SDL_Event event;
    SDL_UserEvent userevent;

    userevent.type = SDL_USEREVENT;
    userevent.code = 0;
    userevent.data1 = NULL;
    userevent.data2 = NULL;

    event.type = SDL_USEREVENT;
    event.user = userevent;

    SDL_PushEvent(&event);
	
	Uint32 used_intervall = interval;
	if ( param != NULL) {
		Uint32 game_interval = *((unsigned int *)param);
		#if debug
			SDL_Log("INPUT: game intervall found: %i\n", game_interval);
		#endif
		if ( game_interval < interval ) {
			#if debug
				SDL_Log("INPUT: update intervall old: %i  new: %i\n", interval, game_interval);
			#endif
			used_intervall = game_interval;
		}	
	}
    return(used_intervall);
}

void tetris_input_update(tetris_input_t *input) {
	SDL_Event e;
	SDL_PollEvent(&e);
	
	Uint32 type = e.type;
	input->state.type = type;
	switch(type) {
		case SDL_KEYDOWN: input->state.keycode = e.key.keysym.sym; 
						  input->state.repeat = e.key.repeat;
						  break;
		default: input->state.keycode = SDLK_UNKNOWN;
				 input->state.repeat = 0; /* no repeat */
	}	
}
