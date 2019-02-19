#include "tetris_app.h"

/* START Private Section */

static bool __tetris_app_init() {
	
	/* Enable standard application logging */
    SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);

    /* Initialize SDL */
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init fail : %s\n", SDL_GetError());
		return false;
	}
	
	/* init font sys */
	if (TTF_Init() < 0) {
		printf("Font opening error : %s\n",TTF_GetError()); 
		return false;
	}
	
	/* init sound sys */
	if (Mix_Init(MIX_INIT_OGG) != (MIX_INIT_OGG)) { 
		printf("Mix_Init: Failed to init required ogg support!\nMix_Init: %s\n", Mix_GetError());
		return false;
	}

	if(IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
		printf("IMG_Init: Failed to init required jpg and png support!\nIMG_Init: %s\n", IMG_GetError());
		return false;
	}
	
	return true;
}

static void __tetris_cleanup_sdl() {
	TTF_Quit();
	Mix_CloseAudio();
	IMG_Quit();
    SDL_Quit();
}

/* EO Private Section */

/* START Public Section */

bool tetris_app_init(tetris_app_t *app, int argc, char **argv) {
	bool init_successfully = true;
	if ( app != NULL ) {
		srand(time(NULL));
		app->state.done = false;
		app->argc = argc;
		app->argv = argv;
		
		init_successfully &= __tetris_app_init();
		init_successfully &= tetris_game_init(&app->game);
		init_successfully &= tetris_input_init(&app->input);
		init_successfully &= tetris_gfx_init(&app->ui.gfx);
		init_successfully &= tetris_ui_init(&app->ui, &app->game);
		init_successfully &= init_tetris_sfx(&app->sfx);
		
	}
	return init_successfully;
}

void tetris_app_run(tetris_app_t *app) {
	
	tetris_game_t *game = &app->game;
	tetris_input_t *input = &app->input;
	tetris_ui_t *ui = &app->ui;
	tetris_sfx_t *sfx = &app->sfx;
	
	clock_t start;
	clock_t end;
	static const clock_t CLOCKS_PER_FRAME = CLOCKS_PER_SEC / 60; 
	static const float ONE_CLOCK = (1.f / CLOCKS_PER_SEC) * 1000; 
	while (app->state.done == false) {
		
		start = clock();
		
		tetris_input_update(input);
		
		tetris_game_update(game, input, &start);
		/* finish game */
		app->state.done = game->state.quit;
		
		tetris_ui_update(ui, game, &start);
		
		tetris_sfx_update(sfx, game);
		
		/* here we should add frame sheduler to 60fps */
		SDL_UpdateWindowSurface(ui->window);
		
		end = clock();
		
		//}
		int delay = fmax((CLOCKS_PER_FRAME - ( end - start )) * ONE_CLOCK, 1);
		//SDL_Log("per second: %i, per frame: %i, start: %i, end: %i, delta: %i in ms: %i",CLOCKS_PER_SEC, CLOCKS_PER_FRAME, start, end, (end - start), delay);
		/* wait 1ms to reduce cpu usage. Here we should use another kind of logic with animations */
		SDL_Delay(delay);
    }
}


void tetris_app_cleanup(tetris_app_t *app) {
	tetris_ui_cleanup(&app->ui);
	tetris_game_cleanup(&app->game);
	__tetris_cleanup_sdl();
}

/* EO Public Section */