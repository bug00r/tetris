#include "tetris_ui.h"

static bool __tetris_ui_init_screen(tetris_ui_t *ui) {

    /* Create window and renderer for given surface */
    ui->window = SDL_CreateWindow("Bug0r's Tetris", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 524, 524, 0);
    if (!ui->window) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Window creation fail : %s\n",SDL_GetError());
        return false;
    }
    ui->win_surface = SDL_GetWindowSurface(ui->window);
    ui->renderer = SDL_CreateSoftwareRenderer(ui->win_surface);
    if (!ui->renderer) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Render creation for surface fail : %s\n",SDL_GetError());
        return false;
    }
	
	SDL_SetRenderDrawColor(ui->renderer, 0x0, 0x0, 0x0, 0xFF);
	SDL_RenderClear(ui->renderer);
	
	return true;
}

static void __print_tetris_game_over_sdl(tetris_ui_t *ui) {

	DRAW_TXT_AT_POS( ui->win_surface, "(= GAME OVER =)",  
					 ui->gfx.score_font, &ui->gfx.color_value, (ui->width - text->w) / 2 , (ui->height - text->h) / 2);
}

static void __print_tetris_pause_sdl(tetris_ui_t *ui) {

	DRAW_TXT_AT_POS( ui->win_surface, "-= PAUSE =-",  
					 ui->gfx.score_font, &ui->gfx.color_value, (ui->width - text->w) / 2 , (ui->height - text->h) / 2);
					 
}

static void __print_tetris_score_label_sdl(tetris_ui_t *ui) {

	DRAW_TXT_AT_POS( ui->win_surface, "Score:",  ui->gfx.score_font, &ui->gfx.color_label, 0, 36);

}

static void __print_tetris_score_sdl(tetris_ui_t *ui, unsigned int score) {

	/* clear text */
	tetris_gfx_draw_rect_rgba(ui->renderer, 0, 54, 126, 18, 0, 0, 0, 255);
	
	DRAW_UINT_TXT_AT_POS( ui->win_surface, score, ui->gfx.score_font, &ui->gfx.color_value, 0, 54);
	
}

static void __print_tetris_level_label_sdl(tetris_ui_t *ui) {

	DRAW_TXT_AT_POS( ui->win_surface, "Level:",  ui->gfx.level_font, &ui->gfx.color_label, 0, 72);

}

static void __print_tetris_level_sdl(tetris_ui_t *ui, unsigned int level) {

	/* clear text */
	tetris_gfx_draw_rect_rgba(ui->renderer, 0, 90, 126, 18, 0, 0, 0, 255);
	
	DRAW_UINT_TXT_AT_POS( ui->win_surface, level, ui->gfx.level_font, &ui->gfx.color_value, 0, 90);
	
}

static void __print_tetris_lines_label_sdl(tetris_ui_t *ui) {

	DRAW_TXT_AT_POS( ui->win_surface, "Lines:",  ui->gfx.level_font, &ui->gfx.color_label, 0, 108);

}

static void __print_tetris_lines_sdl(tetris_ui_t *ui, unsigned int lines) {
	/* clear text */
	tetris_gfx_draw_rect_rgba(ui->renderer, 0, 126, 126, 18, 0, 0, 0, 255);
	
	DRAW_UINT_TXT_AT_POS( ui->win_surface, lines, ui->gfx.level_font, &ui->gfx.color_value, 0, 126);
	
}

static void __print_tetris_headline_sdl(tetris_ui_t *ui) {

	DRAW_TXT_AT_POS( ui->win_surface, "-= Tetris =-", 
					 ui->gfx.headline_font, &ui->gfx.color_headline, (ui->width - text->w) / 2, 0);
}

static void __set_color_for_stone(stone_type_t *stone, float *r, float *g, float *b) {
	switch(*stone) {
		case TETRIS_QUAD: *r = 1.f; *g = 0.5f; *b = 0.25f; break;
		case TETRIS_LINE: *r = .25f; *g = 0.5f; *b = 1.f; break;
		case TETRIS_ROOF: *r = 1.f; *g = 0.25f; *b = 0.5f; break;
		case TETRIS_STEP_LEFT: *r = .5f; *g = 0.25f; *b = 1.f; break;
		case TETRIS_STEP_RIGHT: *r = .5f; *g = 1.f; *b = 0.25f; break;
		case TETRIS_L_LEFT: *r = .25f; *g = 1.f; *b = 0.5f; break;
		case TETRIS_L_RIGHT: *r = 1.f; *g = 0.5f; *b = 0.0f; break;
		default: *r = 0.05f; *g = 0.05f; *b = 0.05f; 
	}
}

static void _print_tetris_stone_cell(tetris_ui_t* ui, 
									 stone_type_t *stone, stone_direction_t *direction, unsigned int cell_index,
									 unsigned int cell_row, unsigned int cell_col,
									 unsigned int x_offset, unsigned int y_offset, unsigned int block_size_w, unsigned int block_size_h){
	
	static const unsigned int border_size = 1;
	
	unsigned int x = (cell_col * block_size_w) + x_offset;
	unsigned int y = (cell_row * block_size_h) + y_offset;
	
	SDL_Renderer *renderer = ui->renderer;
	
	if ( ui->texture_mode == -1 ) {
	
		float r, g, b;
		__set_color_for_stone(stone, &r, &g, &b);
		
		tetris_gfx_draw_rect_rgba(renderer, x + border_size, y + border_size, block_size_w - border_size, 
								  block_size_h - border_size, 
								  (unsigned char)(255.f * r), (unsigned char)(255.f * g),(unsigned char)(255.f * b), 255);

		
		tetris_gfx_draw_rect_outline_rgba(renderer, x, y, block_size_w, block_size_h, 
						   (unsigned char)32, (unsigned char)32, (unsigned char)32, 255);
	} else {
		/* drawing cell with another algorithm see spec */
		/* y * width + x */
		SDL_Rect src;
		//src.x = *stone * block_size_w;
		src.x = (*direction * (4*block_size_w)) + (cell_index * block_size_w);
		//src.y = (ui->texture_cnt_offset * ui->texture_mode) + ui->texture_offset_y;
		src.y = *stone * block_size_h;
		src.w = block_size_w;
		src.h = block_size_h;
		
		SDL_Rect dest;
		dest.x = x;
		dest.y = y;
		dest.w = block_size_w;
		dest.h = block_size_h;
		
		SDL_BlitSurface(ui->gfx.texture, &src, ui->win_surface, &dest);
		
	}
					   
}

static void __print_tetris_stone(tetris_ui_t* ui, stone_type_t *stone, stone_direction_t *direction,
									unsigned int x, unsigned int y, unsigned int block_size_w, unsigned int block_size_h, bool delete) {

	const stone_t *cur_stone = &tetris_stones[*stone].stones[*direction];
	
	stone_type_t used_stone = *stone;
	if(delete) {
		used_stone = TETRIS_NO_STONE;
	}
	
	for (unsigned int cell_no = 4; cell_no--; ) {
	
		stone_data_t cur_cell = cur_stone->cells[cell_no];
		
		_print_tetris_stone_cell(ui, &used_stone, direction, cell_no, cur_cell.row, cur_cell.col, x, y, block_size_w, block_size_h);
	
	}
}

static void __print_tetris_stone_in_game(tetris_ui_t* ui, tetris_game_t* game) {
	tetris_t *tetris = game->tetris;
	
	if ( tetris->active_stone.type == TETRIS_NO_STONE ) return;
	
	const unsigned int block_size_w = ui->block_size_w;
	const unsigned int block_size_h = ui->block_size_h;
	const unsigned int x_offset = ui->start_field_x;
	const unsigned int y_offset = ui->start_field_y;
	
	/* deleting previouse position */
	stone_position_t cur_pos = game->last_stone.position;
	stone_type_t cur_stone = game->last_stone.type;
	stone_direction_t direction = game->last_stone.direction;
	
	if ( cur_stone != TETRIS_NO_STONE ) {
	
		__print_tetris_stone(ui, &cur_stone, &direction, (cur_pos.col * block_size_w) + x_offset,
										 (cur_pos.row * block_size_h) + y_offset, block_size_w, block_size_h, true);	
	
	}
	
	/* drawing actual stone */
	cur_pos = tetris->active_stone.position;
	cur_stone = tetris->active_stone.type;
	direction = tetris->active_stone.direction;
	
	__print_tetris_stone(ui, &cur_stone, &direction, (cur_pos.col * block_size_w) + x_offset,
									 (cur_pos.row * block_size_h) + y_offset, block_size_w, block_size_h, false);									 
}

static void __print_tetris_next_stone_sdl(tetris_ui_t* ui, tetris_game_t* game) {
	
	SDL_Surface * text = tetris_gfx_create_text("Next:", ui->gfx.level_font, &ui->gfx.color_label);
	
	tetris_gfx_draw_text_at_position(text ,ui->win_surface, 0, 170);
	
	SDL_FreeSurface(text); /* No use anymore, could be cached because its a label, need text cache */
	
	tetris_gfx_draw_rect_rgba(ui->renderer, 20, 200, 96, 96, 0, 0, 0, 255);
	
	stone_direction_t direction = TETRIS_NORTH;
	
	__print_tetris_stone(ui, &game->next_stone, &direction, 20, 200, 24, 24, false);
}

static void __print_tetris_field_sdl(tetris_ui_t* ui, tetris_game_t* game) {

	tetris_t * tetris = game->tetris;
	unsigned int cols = tetris->field_size.cols;
	unsigned int rows = tetris->field_size.rows;
	
	const unsigned int block_size_w = ui->block_size_w;
	const unsigned int block_size_h = ui->block_size_h;
	const unsigned int x_offset = ui->start_field_x;
	const unsigned int y_offset = ui->start_field_y;
	
	/* delete backgound from field*/
	tetris_gfx_draw_rect_rgba(ui->renderer, x_offset, y_offset, 
							  ui->field_width_px, ui->field_height_px,
							  0, 0, 0, 255);
	
	/* print field */
	for ( unsigned int row = rows ; row--; ) {
		for ( unsigned int col = cols ; col--; ) {

			//unsigned int field_value = tetris->field[row * cols + col];
			field_cell_t *field_value = &tetris->field[row * cols + col];
			
			_print_tetris_stone_cell(ui, &field_value->stone_type, &field_value->stone_direction, field_value->stone_cell, 
											row, col, x_offset, y_offset, block_size_w, block_size_h);
			//_print_tetris_stone_cell(ui, field_value, row, col, x_offset, y_offset, block_size_w, block_size_h);
			
		}
	}

}

static void __tetris_ui_draw_labels(tetris_ui_t *ui, tetris_game_t* game) {
	__print_tetris_headline_sdl(ui);
	__print_tetris_score_label_sdl(ui);
	__print_tetris_level_label_sdl(ui);
	__print_tetris_lines_label_sdl(ui);
	__print_tetris_field_sdl(ui, game);
}

static void __tetris_ui_draw_scores(tetris_ui_t *ui, tetris_game_t* game) {
	__print_tetris_score_sdl(ui, game->score);
	__print_tetris_level_sdl(ui, game->level);
	__print_tetris_lines_sdl(ui, game->lines);
}

static void __tetris_ui_recalc_field(tetris_ui_t* ui, tetris_game_t* game) {
	const tetris_t * tetris = game->tetris;
	unsigned int cols = tetris->field_size.cols;
	unsigned int rows = tetris->field_size.rows;

	ui->block_size_w = ui->field_width_px / cols;
	ui->block_size_h = ui->field_height_px / rows;
	ui->start_field_x = (ui->width - (cols * ui->block_size_w)) / 2; 
	ui->start_field_y = (ui->height - (rows * ui->block_size_h));	 
}

bool tetris_ui_init(tetris_ui_t *ui, tetris_game_t* game) {
	bool ini_success = false;
	if (ui != NULL) {
		ui->field_width_px = 240;
		ui->field_height_px = 480;
		ui->width = 524;
		ui->height = 524;
		
		ui->texture_mode = -1;
		ui->texture_cnt_offset = 192;
		ui->texture_mode_max = (ui->gfx.texture->h / 192) - 1;
		
		ui->texture_offset_y = 0;
		
		ini_success = __tetris_ui_init_screen(ui);
		
		__tetris_ui_recalc_field(ui, game);
		
		__tetris_ui_draw_labels(ui, game);
		
		__tetris_ui_draw_scores(ui, game);
		
		SDL_UpdateWindowSurface(ui->window);
	}
	return ini_success;
}

static void __tetris_ui_next_texture(tetris_ui_t *ui) {
	if (ui->texture_mode != ui->texture_mode_max) {
		ui->texture_mode++;
	} else {
		ui->texture_mode = -1;
	}
}

static void __ui_update_level_score_lines(tetris_ui_t *ui, tetris_game_t *game) {
	tetris_game_state_t *last_state = &game->last_state;
	if ( last_state->got_full_lines ) {
		__print_tetris_lines_sdl(ui, game->lines);

		if (last_state->got_level_up) {
			__print_tetris_level_sdl(ui, game->level);
		}
	}
	#if debug
		SDL_Log("UI: print score of: %i\n", game->score);
	#endif
	__print_tetris_score_sdl(ui, game->score);
}

static void __update_headline_animation(tetris_ui_t *ui) {

	ui->gfx.color_headline.r += (ui->gfx.color_headline.r >= 255 ? -2 : 2);
	ui->gfx.color_headline.b += (ui->gfx.color_headline.g >= 255 ? -2 : 2);
	ui->gfx.color_headline.g += (ui->gfx.color_headline.b >= 255 ? -2 : 2);
	
	DRAW_TXT_AT_POS( ui->win_surface, "-= Tetris =-", 
				 ui->gfx.headline_font, &ui->gfx.color_headline, (ui->width - text->w) / 2, 0);
					 
}

static void __tetris_ui_removing_lines(tetris_ui_t *ui, tetris_game_t *game) {
	tetris_game_state_t *state = &game->state;
	tetris_game_state_t *last_state = &game->last_state;
	SDL_Color *color = &ui->gfx.state.line_remove_color;
	
	if ( !last_state->removing_full_lines && state->removing_full_lines ) {
		color->r = 0;
		color->g = 0;
		color->b = 0;
		color->a = 255;
	} else {
		color->r += (color->r >= 255 ? -60 : 60);
	}
	tetris_full_lines_t *full_lines = &game->state.full_lines;
	for ( unsigned int cnt = full_lines->cnt; cnt--; ) {
		tetris_gfx_draw_rect_rgba(ui->renderer, ui->start_field_x, ui->start_field_y + (full_lines->lines[cnt] * ui->block_size_h), 
								  ui->field_width_px, ui->block_size_h, color->r, color->g, color->b, color->a);
	}
	
}

void tetris_ui_update(tetris_ui_t *ui, tetris_game_t *game, clock_t *ticks) {
	UNUSED(ticks);
	tetris_game_state_t *state = &game->state;
	tetris_game_state_t *last_state = &game->last_state;
	
	__update_headline_animation(ui);
	
	if ( state->removing_full_lines ) {
		__tetris_ui_removing_lines(ui, game);
		return;
	}
	
	if ( last_state->removing_full_lines && !state->removing_full_lines ) {
		__print_tetris_stone_in_game(ui, game);
		__ui_update_level_score_lines(ui, game);
		__print_tetris_next_stone_sdl(ui, game);
		__print_tetris_field_sdl(ui, game);
	}
	
	if ( last_state->stone_was_landed || state->stone_was_moved || state->stone_was_rotated ) {
		__print_tetris_stone_in_game(ui, game);
		return;
	}
	
	if ( !state->stone_was_moved && state->stone_was_landed ) {
		__ui_update_level_score_lines(ui, game);
		__print_tetris_next_stone_sdl(ui, game);
		__print_tetris_field_sdl(ui, game);
		return;
	}
	
	if ( state->was_paused ) {
		__print_tetris_pause_sdl(ui);
		return;
	}
	
	if ( last_state->was_paused && !state->was_paused ) {
		__print_tetris_field_sdl(ui, game);
		__print_tetris_stone_in_game(ui, game);
		return;
	}
	
	if ( state->new_game ) {
		__tetris_ui_recalc_field(ui, game);
		__print_tetris_score_sdl(ui, game->score);
		__print_tetris_level_sdl(ui, game->level);
		__print_tetris_lines_sdl(ui, game->lines);
		__print_tetris_next_stone_sdl(ui, game);
		
		__print_tetris_field_sdl(ui, game);

		__print_tetris_stone_in_game(ui, game);
		return;
	}
	
	if ( state->change_texture ) {
		__tetris_ui_next_texture(ui);
		__print_tetris_next_stone_sdl(ui, game);			
		__print_tetris_field_sdl(ui, game);
		__print_tetris_stone_in_game(ui, game);
		return;
	}
	
	if ( state->game_over ) {
		__print_tetris_stone_in_game(ui, game);
		__print_tetris_game_over_sdl(ui);
	}
	
}

void tetris_ui_cleanup(tetris_ui_t *ui) {
	UNUSED(ui);
}


