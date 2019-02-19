#include "tetris.h"

const stone_position_t TETRIS_NO_POSITION = {UINT_MAX, UINT_MAX}; 

const stone_direction_t tetris_next_direction_of[] = {
	TETRIS_EAST, TETRIS_SOUTH, TETRIS_WEST, TETRIS_NORTH
};

const stone_collection_t tetris_stones[] = {
	//no stone
	{{ 
		{.cells={{0,0},{0,0},{0,0},{0,0}}}, {.cells={{0,0},{0,0},{0,0},{0,0}}},
		{.cells={{0,0},{0,0},{0,0},{0,0}}}, {.cells={{0,0},{0,0},{0,0},{0,0}}}
	}},
	//quads, all the same
	{{ 
		{.cells={{0,0},{0,1},{1,0},{1,1}}}, {.cells={{0,0},{0,1},{1,0},{1,1}}},
		{.cells={{0,0},{0,1},{1,0},{1,1}}}, {.cells={{0,0},{0,1},{1,0},{1,1}}}
	}},
	//line
	{{ 
		{.cells={{0,0},{1,0},{2,0},{3,0}}}, {.cells={{0,0},{0,1},{0,2},{0,3}}},
		{.cells={{0,0},{1,0},{2,0},{3,0}}}, {.cells={{0,0},{0,1},{0,2},{0,3}}}
	}},
	//roof
	{{
		{.cells={{0,1},{1,0},{1,1},{1,2}}}, {.cells={{0,1},{1,1},{2,1},{1,2}}},
		{.cells={{1,0},{1,1},{1,2},{2,1}}}, {.cells={{0,1},{1,1},{2,1},{1,0}}}
	}},
	//step left
	{{
		{.cells={{0,0},{0,1},{1,1},{1,2}}}, {.cells={{0,1},{1,0},{1,1},{2,0}}},
		{.cells={{0,0},{0,1},{1,1},{1,2}}}, {.cells={{0,1},{1,0},{1,1},{2,0}}}
	}},
	//step right
	{{
		{.cells={{0,1},{0,2},{1,0},{1,1}}}, {.cells={{0,0},{1,0},{1,1},{2,1}}},
		{.cells={{0,1},{0,2},{1,0},{1,1}}}, {.cells={{0,0},{1,0},{1,1},{2,1}}}
	}},
	//L left
	{{
		{.cells={{0,0},{0,1},{1,1},{2,1}}}, {.cells={{0,2},{1,0},{1,1},{1,2}}},
		{.cells={{0,0},{1,0},{2,0},{2,1}}}, {.cells={{0,0},{0,1},{0,2},{1,0}}}
	}},
	//L right
	{{
		{.cells={{0,0},{0,1},{1,0},{2,0}}}, {.cells={{0,0},{0,1},{0,2},{1,2}}},
		{.cells={{0,1},{1,1},{2,0},{2,1}}}, {.cells={{0,0},{1,0},{1,1},{1,2}}}
	}}
};

tetris_t* tetris_new() {
	tetris_t * new_tetris = malloc(sizeof(tetris_t));
	
	//fetch new field size and set to tetris game
	tetris_field_size_t field_size = {20, 10};
	new_tetris->field_type = TETRIS_NORMAL;
	new_tetris->field_size = field_size;
	
	//allocate fieldsize depend memory and init field with zeros
	unsigned int field_buffer = field_size.rows*field_size.cols;
	//new_tetris->field = malloc(field_buffer * sizeof(unsigned int));field_cell_t
	new_tetris->field = malloc(field_buffer * sizeof(field_cell_t));
	tetris_reset(new_tetris);
	
	return new_tetris;
}

void tetris_free(tetris_t** _tetris) {
	if(_tetris != NULL && *_tetris != NULL) {
		tetris_t* tetris = *_tetris;
		free(tetris->field);
		free(tetris);
		*_tetris = NULL;
	}
}

void tetris_reset(tetris_t* tetris) {
	if ( tetris != NULL ) {
		unsigned int field_buffer = tetris->field_size.rows * tetris->field_size.cols;
		memset(tetris->field, 0, field_buffer * sizeof(field_cell_t));
		tetris->active_stone.direction 	= TETRIS_NORTH;
		tetris->active_stone.type 		= TETRIS_NO_STONE;
		tetris->active_stone.position = TETRIS_NO_POSITION;
		tetris->state = TETRIS_NOT_RUNNING;
	}
}

void tetris_start(tetris_t* tetris) {
	if ( tetris != NULL && tetris->state == TETRIS_NOT_RUNNING) {
		tetris_reset(tetris);
		tetris->state = TETRIS_RUNS;
	}
}

void tetris_pause(tetris_t* tetris) {
	if ( tetris != NULL && tetris->state == TETRIS_RUNS) {
		tetris->state = TETRIS_PAUSE;
	}
}

void tetris_continue(tetris_t* tetris) {
	if ( tetris != NULL && tetris->state == TETRIS_PAUSE) {
		tetris->state = TETRIS_RUNS;
	}
}

void tetris_stop(tetris_t* tetris) {
	if (tetris != NULL && 
	   (tetris->state == TETRIS_PAUSE || tetris->state == TETRIS_RUNS)) {
		tetris->state = TETRIS_NOT_RUNNING;
	}
}

bool tetris_was_paused(tetris_t* tetris) {
	return (tetris != NULL && tetris->state == TETRIS_PAUSE);
}

bool tetris_was_started(tetris_t* tetris) {
	return (tetris != NULL && (tetris->state == TETRIS_PAUSE || tetris->state == TETRIS_RUNS) );
}

bool tetris_is_running(tetris_t* tetris) {
	return (tetris != NULL && tetris->state == TETRIS_RUNS);
}

static int random_int(const int lnum, const int hnum){
	return lnum + ((int)rand()/((int)(RAND_MAX)/(hnum-lnum)));
}

static bool __cell_fits_in_field(const tetris_t* tetris, const int row, const int col) {
	//row and col are inside tetris field
	if ( row < 0 || col < 0 || (unsigned int)row >= tetris->field_size.rows || (unsigned int)col >= tetris->field_size.cols ) return false;
	return tetris->field[row * tetris->field_size.cols + col].stone_type == 0;
}

static bool __stone_fit_int_position(const tetris_t* tetris, stone_t *stone) {
	bool fit = true;
	const stone_position_t cur_pos = tetris->active_stone.position;
	const stone_t *cur_stone = stone;//tetris_stones[tetris->active_stone.type].stones[tetris->active_stone.direction];
	for (unsigned int cell_no = 4; cell_no--; ) {
		stone_data_t cur_cell = cur_stone->cells[cell_no];
		int new_row = (int)cur_pos.row + cur_cell.row;
		int new_col = (int)cur_pos.col + cur_cell.col;
		if ( !__cell_fits_in_field(tetris, new_row, new_col) ) {
			fit = false;
			break;
		}
	}
	return fit;
}

static void __create_new_tetris_stone(stone_type_t stone, tetris_t* tetris, tetris_create_stone_response_t* response) {
	response->type = TETRIS_NO_STONE;
	response->position = TETRIS_NO_POSITION;
	if ( tetris_is_running(tetris) ) {
		response->type = stone;
		response->position.row = 0;
		response->direction = TETRIS_NORTH;
		response->position.col = (tetris->field_size.cols/2)-1;
	};
	tetris->active_stone.type = response->type;
	tetris->active_stone.direction = TETRIS_NORTH;
	tetris->active_stone.position = response->position;
	
	stone_t new_stone = tetris_stones[tetris->active_stone.type].stones[TETRIS_NORTH];
	if ( !__stone_fit_int_position(tetris, &new_stone) ) {
		response->type = TETRIS_MAX_STONE;
	}
	
}

stone_type_t tetris_random_stone() {
	return (stone_type_t)random_int(TETRIS_QUAD, TETRIS_MAX_STONE);
}

void tetris_create_stone_random(tetris_t* tetris, tetris_create_stone_response_t* response) {
	stone_type_t new_stone = tetris_random_stone();
	__create_new_tetris_stone(new_stone, tetris, response);
}

void tetris_create_stone(stone_type_t stone, tetris_t* tetris, tetris_create_stone_response_t* response){
	stone_type_t used_stone = stone;
	if ( used_stone <= TETRIS_NO_STONE || used_stone >= TETRIS_MAX_STONE ) used_stone = TETRIS_QUAD;
	__create_new_tetris_stone(used_stone, tetris, response);
}

void tetris_rotate_stone(tetris_t* tetris, tetris_move_response_t * response){

	if (tetris != NULL) {
	
		stone_direction_t new_direction = tetris_next_direction_of[tetris->active_stone.direction];
		stone_t next_stone = tetris_stones[tetris->active_stone.type].stones[new_direction];
		
		if ( __stone_fit_int_position(tetris, &next_stone) ) {
			tetris->active_stone.direction = new_direction;
			
			if ( response->full_lines != NULL ) {
				tetris_reset_move_response(tetris, response);
			}
			
			response->was_rotated = true;
			
		} else {
			response->was_rotated = false;
		}
	}
	
}

typedef enum {
	TETRIS_MOVED 	 	 		= 0, 	/* Stone was moved */
	TETRIS_NOT_MOVED_DOWN 		= 1,	/* Stone can't move because end row was reached */
	TETRIS_NOT_MOVE_LR 			= 2,	/* Stone can't move because end of column reached */
	TETRIS_NOT_MOVE_RESET		= 3		/* Stone can't move because Stone was reset, you have to add a new one */
} tetris_move_state_t;

static tetris_move_state_t __stone_move(tetris_t* tetris, int move_row, int move_col) {

		tetris_move_state_t move_state = TETRIS_MOVED;
		if ( tetris->active_stone.type > TETRIS_NO_STONE && 
			 tetris->active_stone.type < TETRIS_MAX_STONE &&
			 &tetris->active_stone.position != &TETRIS_NO_POSITION ){

			stone_position_t *cur_pos = &tetris->active_stone.position;
			int new_pos_row = cur_pos->row + move_row;
			int new_pos_col = cur_pos->col + move_col;
			
			const stone_t *cur_stone = &tetris_stones[tetris->active_stone.type].stones[tetris->active_stone.direction];
			
			move_state = TETRIS_MOVED;
			for (unsigned int cell_no = 4; cell_no--; ) {
				stone_data_t cur_cell = cur_stone->cells[cell_no];
				int _new_row_cell = new_pos_row + cur_cell.row;
				int _new_col_cell = new_pos_col + cur_cell.col;
				if ( !__cell_fits_in_field(tetris, _new_row_cell, _new_col_cell) ) {
					if ( move_row != 0 ) {
						move_state = TETRIS_NOT_MOVED_DOWN;
					} else  if ( move_col != 0 ) {
						move_state = TETRIS_NOT_MOVE_LR;
					}
				}
			}
			
			if (move_state == TETRIS_MOVED) {
				cur_pos->row = (unsigned int) new_pos_row;
				cur_pos->col = (unsigned int) new_pos_col;
			}
		
		}
		return move_state;
}

void tetris_move_stone_left(tetris_t* tetris, tetris_move_response_t * response){

	if (tetris != NULL) {
		tetris_move_state_t moved_state = __stone_move(tetris, 0, -1);
		
		if ( response->full_lines != NULL ) {
			tetris_reset_move_response(tetris, response);
		}
		
		response->was_moved = (moved_state == TETRIS_MOVED);
		
	}
	
}

void tetris_move_stone_right(tetris_t* tetris,  tetris_move_response_t * response){

	if (tetris != NULL) {
		tetris_move_state_t moved_state = __stone_move(tetris, 0, 1);
		
		if ( response->full_lines != NULL ) {
			tetris_reset_move_response(tetris, response);
		}
		
		response->was_moved = (moved_state == TETRIS_MOVED);
		 
	}
	
}

static void __persist_stone_in_field(tetris_t* tetris) {

	const stone_position_t cur_pos = tetris->active_stone.position;
	const stone_t *cur_stone = &tetris_stones[tetris->active_stone.type].stones[tetris->active_stone.direction];
	field_cell_t *field = tetris->field;
	for (unsigned int cell_no = 4; cell_no--; ) {
		stone_data_t cur_cell = cur_stone->cells[cell_no];
		int new_row = (int)cur_pos.row + cur_cell.row;
		int new_col = (int)cur_pos.col + cur_cell.col;
		//tetris->field[new_row * tetris->field_size.cols + new_col] = tetris->active_stone.type;
		field_cell_t *cur_field_cell = &field[new_row * tetris->field_size.cols + new_col];
		cur_field_cell->stone_type = tetris->active_stone.type;
		cur_field_cell->stone_direction = tetris->active_stone.direction;
		cur_field_cell->stone_cell = cell_no;
	}
	
}

static void __reset_stone_from_field(tetris_t* tetris) {

	if ( tetris != NULL ) {
		tetris->active_stone.type = TETRIS_NO_STONE;
		tetris->active_stone.direction = TETRIS_NORTH;
		tetris->active_stone.position = TETRIS_NO_POSITION;
	}
	
}

static tetris_full_lines_t* tetris_full_lines_new() {

	tetris_full_lines_t * new_full_lines = malloc(sizeof(tetris_full_lines_t));
	
	new_full_lines->lines[0] = -1;
	new_full_lines->lines[1] = -1;
	new_full_lines->lines[2] = -1;
	new_full_lines->lines[3] = -1;
	
	new_full_lines->next = NULL;
	
	new_full_lines->cnt = 0;
	
	return new_full_lines;
}

static void __tetris_full_lines_free(tetris_full_lines_t** full_lines) {

	tetris_full_lines_t * cur_full_lines = *full_lines;
	
	if(*full_lines != NULL) {
		tetris_full_lines_t* next = cur_full_lines->next;
		__tetris_full_lines_free(&next);
		free(cur_full_lines);
		full_lines = NULL;
	}
	
}

static tetris_full_lines_t* __check_full_lines(tetris_t* tetris) {
	
	unsigned int found_full_lines = 0;
	int full_lines_index[4] = {-1, -1, -1, -1};
	
	unsigned int cur_full_line = -1;
	for ( unsigned int row = tetris->field_size.rows ; row--;) {
		bool row_is_full = true;
		for ( unsigned int col = tetris->field_size.cols; col--; ) {
			if ( tetris->field[row * tetris->field_size.cols + col].stone_type == 0 ) {
				row_is_full = false;
				break;
			}
		}

		if (row_is_full) {
			++found_full_lines;
			full_lines_index[++cur_full_line] = row;
		}
	}
	
	tetris_full_lines_t * full_lines = NULL;
	if (found_full_lines > 0) {
	
		full_lines = tetris_full_lines_new();
		full_lines->cnt = found_full_lines;
		
		full_lines->lines[0] = full_lines_index[0];
		full_lines->lines[1] = full_lines_index[1];
		full_lines->lines[2] = full_lines_index[2];
		full_lines->lines[3] = full_lines_index[3];
		
	}
	
	return full_lines;
}

static void __delete_full_lines(tetris_t* tetris, tetris_full_lines_t * cur_full_lines) {

	if ( cur_full_lines != NULL && cur_full_lines->cnt > 0 ) {
		
		for (unsigned int cnt_line = cur_full_lines->cnt; cnt_line--;) {
		
			unsigned int cur_row = cur_full_lines->lines[cnt_line];
			
			for ( unsigned int col = tetris->field_size.cols; col--; ) {
				tetris->field[cur_row * tetris->field_size.cols + col].stone_type = 0;
			}
			
		}
		
	}
}

static void __falling_down_other_lines(tetris_t* tetris, tetris_full_lines_t * cur_full_lines) {

	const unsigned int field_cols = tetris->field_size.cols;
	
	for (unsigned int cnt_line = cur_full_lines->cnt; cnt_line--;) {
	
		unsigned int cur_start_row = cur_full_lines->lines[cnt_line];
		
		for ( unsigned int col = tetris->field_size.cols; col--; ) {
			
			field_cell_t *field = tetris->field;
			
			for ( unsigned int row = cur_start_row; row > 0; --row) {
				field_cell_t *target = &field[row * field_cols + col];
				field_cell_t *source = &field[(row-1) * field_cols + col];
				target->stone_type = source->stone_type;
				target->stone_direction = source->stone_direction;
				target->stone_cell = source->stone_cell;
				//field[row * field_cols + col] = field[(row-1) * field_cols + col];
			}
			
			field_cell_t *first = &field[0];
			first->stone_type = 0;
			first->stone_direction = TETRIS_NORTH;
			first->stone_cell = 0;
		
		}
	}	
}

void tetris_move_stone_down(tetris_t* tetris, tetris_move_response_t * response){

	tetris_full_lines_t * full_lines = NULL;
	
	if (tetris != NULL) {
	
		tetris_move_state_t moved_state = __stone_move(tetris, 1, 0);
		response->was_moved = (moved_state == TETRIS_MOVED);
		
		if(!response->was_moved && (moved_state == TETRIS_NOT_MOVED_DOWN)) {
		
			__persist_stone_in_field(tetris);
			
			__reset_stone_from_field(tetris);
			
			tetris_full_lines_t * cur_full_lines = NULL;
			while ((cur_full_lines = __check_full_lines(tetris)) != NULL) {
			
				if (full_lines == NULL) {
					full_lines = cur_full_lines;
				} else {
					full_lines->next = cur_full_lines;
					full_lines = cur_full_lines;
				}
				
				__delete_full_lines(tetris, cur_full_lines);
				
				__falling_down_other_lines(tetris, cur_full_lines);
			}
		}
		
	}
	response->full_lines = full_lines;
}

void tetris_reset_move_response(tetris_t* tetris, tetris_move_response_t * response) {
	
	if (tetris != NULL && response != NULL) {
	
		response->was_moved 	= false;
		response->was_rotated 	= false;
		response->stone			= tetris->active_stone;
		
		__tetris_full_lines_free(&response->full_lines);
	}
	
}

