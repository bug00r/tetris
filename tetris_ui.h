#ifndef TETRIS_UI_H
#define TETRIS_UI_H

#ifndef M_PI
	#define M_PI 3.14159265358979323846264338327
#endif

#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>

#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"

#include "tetris_gfx.h"
#include "tetris_game.h"
#include "tetris_defs.h"

#ifndef DRAW_TXT_AT_POS
	#define DRAW_TXT_AT_POS(win, txt, font, col, _x,_y) \
		SDL_Surface * text = tetris_gfx_create_text((txt), (font), (col)); \
		tetris_gfx_draw_text_at_position(text ,(win), _x, _y); \
		SDL_FreeSurface(text)
#endif

#ifndef DRAW_UINT_TXT_AT_POS
	#define DRAW_UINT_TXT_AT_POS(win, txt, font, col, _x,_y) \
		SDL_Surface * text = tetris_gfx_create_uint_text((txt), (font), (col)); \
		tetris_gfx_draw_text_at_position(text ,(win), _x, _y); \
		SDL_FreeSurface(text)
#endif

typedef struct {
	tetris_gfx_t		gfx;
	SDL_Window 			*window;		
	SDL_Surface 		*win_surface;
	SDL_Renderer 		*renderer; 
	unsigned int 		field_width_px;   //SDL_UI Future: tetris_ui_config
	unsigned int 		field_height_px;  //SDL_UI Future: tetris_ui_config
	int 				width;			
	int 				height;			
	unsigned int 		block_size_w; 
	unsigned int 		block_size_h; 
	unsigned int 		start_field_x;
	unsigned int 		start_field_y;
	unsigned int 		texture_offset_y;
	int 				texture_mode; 			/* -1 normal drawing, 0 first texture */
    int 				texture_mode_max; 		/* -1 normal drawing, 0 first texture, 1 second.... */
	int 				texture_cnt_offset;
} tetris_ui_t;

bool tetris_ui_init(tetris_ui_t *ui, tetris_game_t* game);
void tetris_ui_cleanup(tetris_ui_t *ui);

void tetris_ui_update(tetris_ui_t *ui, tetris_game_t *game, clock_t *ticks);

#endif