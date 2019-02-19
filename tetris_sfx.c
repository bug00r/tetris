#include "tetris_sfx.h"

EXTERN_BLOB(tetris, ogg);
EXTERN_BLOB(tetris2, ogg);
EXTERN_BLOB(tetris3, ogg);
EXTERN_BLOB(tetris4, ogg);

bool init_tetris_sfx(tetris_sfx_t *sfx) {

	Mix_AllocateChannels(32);
	//Initialize SDL_mixer 
	if( Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1 ) {
		printf("Audio init error : %s", Mix_GetError());
        return false; 
	}
	
	SDL_RWops* sfx_mem = SDL_RWFromMem(&_binary_tetris_ogg_start, (intptr_t)&_binary_tetris_ogg_size);
    if (sfx_mem == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Load sfx from memory fail : %s\n",SDL_GetError());
        return false;
    }

	//sfx->sfx = Mix_LoadMUSType_RW(sfx_mem, MUS_MOD, 0);
	sfx->sfx = Mix_LoadMUSType_RW(sfx_mem, MUS_OGG, 0);
	//tetris_app->sfx = Mix_LoadMUS("tetris.ogg");
	//tetris_app->sfx = Mix_LoadMUSType_RW(sfx_mem, MUS_OGG, 0);
	
	if(sfx->sfx == NULL) {
		printf("Mix_LoadMUS_RW: %s\n", Mix_GetError());
	}
	
	SDL_RWops* sfx_mem2 = SDL_RWFromMem(&_binary_tetris2_ogg_start, (intptr_t)&_binary_tetris2_ogg_size);
    if (sfx_mem2 == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Load sfx2 from memory fail : %s\n",SDL_GetError());
        return false;
    }
	//SDLCALL Mix_QuickLoad_RAW(Uint8 *mem, Uint32 len)
	sfx->sfx2 = Mix_LoadWAV_RW(sfx_mem2, 0);
	//tetris_app->sfx2 = Mix_QuickLoad_RAW(&_binary_tetris2_ogg_start, (int)&_binary_tetris2_ogg_size);
	if(!sfx->sfx2) {
		printf("tetris_app->sfx2: %s\n", Mix_GetError());
		return false;
	}
	
	SDL_RWops* sfx_mem3 = SDL_RWFromMem(&_binary_tetris3_ogg_start, (intptr_t)&_binary_tetris3_ogg_size);
    if (sfx_mem3 == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Load sfx3 from memory fail : %s\n",SDL_GetError());
        return false;
    }

	sfx->sfx3 = Mix_LoadWAV_RW(sfx_mem3, 0);
	if(!sfx->sfx3) {
		printf("tetris_app->sfx3: %s\n", Mix_GetError());
		return false;
	}
	
	SDL_RWops* sfx_mem4 = SDL_RWFromMem(&_binary_tetris4_ogg_start, (intptr_t)&_binary_tetris4_ogg_size);
    if (sfx_mem4 == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Load sfx3 from memory fail : %s\n",SDL_GetError());
        return false;
    }

	sfx->sfx4 = Mix_LoadWAV_RW(sfx_mem4, 0);
	if(!sfx->sfx4) {
		printf("tetris_app->sfx4: %s\n", Mix_GetError());
		return false;
	}
	
	return true;
}

static void __tetris_sfx_play_music_chunk(Mix_Chunk *chunk) {
	if( Mix_PlayChannel( -1, chunk, 0 ) == -1 ) { 
		printf("Error play chnk: %s\n", Mix_GetError());
	} else {
		Mix_VolumeChunk(chunk, MIX_MAX_VOLUME/4);
	}
}

static void __tetris_sfx_play_music(Mix_Music *music) {
	
	
	if( Mix_PlayingMusic() == 0 ) { 
	//Play the music 
		if( music != NULL && (Mix_PlayMusic( music, 0 ) == -1) ) {
			printf("Error play music: %s\n", Mix_GetError());
		} 
		Mix_VolumeMusic(MIX_MAX_VOLUME/4);
	}
}

void tetris_sfx_update(tetris_sfx_t *sfx, tetris_game_t *game) {
	const tetris_game_state_t *state = &game->state;
	const tetris_game_state_t *last_state = &game->last_state;

	if ( !state->stone_was_moved && state->stone_was_landed ) {
		/* playing was landed */
		if ( !state->removing_full_lines ) {
			__tetris_sfx_play_music_chunk(sfx->sfx3);
		}
		
		/* playing full lines chunk */
		if ( !last_state->removing_full_lines && state->got_full_lines ) {
			__tetris_sfx_play_music_chunk(sfx->sfx4);
		}
		//if (state->got_level_up) {
		//	//here we could play sound
		//}
	}
	
	if ( state->stone_was_rotated ) {
		__tetris_sfx_play_music_chunk(sfx->sfx2);
	}
	
	__tetris_sfx_play_music(sfx->sfx);
	
}

