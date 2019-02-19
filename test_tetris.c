#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include "tetris.h"

static void _test_tetris_alloc_() {
	#if debug == 1 
		printf(">>>\n%s => %s\n", __FILE__, __func__);
	#endif
	tetris_t* tetris = tetris_new(555);
	assert(tetris->field_size.rows == 20);
	assert(tetris->field_size.cols == 10);
	assert(tetris->active_stone.type == TETRIS_NO_STONE);
	assert(tetris->active_stone.direction == TETRIS_NORTH);
	assert(&tetris->active_stone.position != &TETRIS_NO_POSITION);
	assert(tetris->active_stone.position.row == TETRIS_NO_POSITION.row);
	assert(tetris->active_stone.position.col == TETRIS_NO_POSITION.col);
	assert(tetris->active_stone.position.row == UINT_MAX);
	assert(tetris->active_stone.position.col == UINT_MAX);
	assert(tetris->state == TETRIS_NOT_RUNNING);
	tetris_free(&tetris);
	assert(tetris == NULL);
	
	tetris = tetris_new(-44);
	assert(tetris->field_size.rows == 20);
	assert(tetris->field_size.cols == 10);
	tetris_free(&tetris);
	assert(tetris == NULL);
	
	tetris = tetris_new(TETRIS_NORMAL);
	assert(tetris->field_size.rows == 20);
	assert(tetris->field_size.cols == 10);
	tetris_free(&tetris);
	assert(tetris == NULL);
	
	#if debug == 1 
		printf("<<<\n");
	#endif
}

static void _test_tetris_commands() {
	#if debug == 1 
		printf(">>>\n%s => %s\n", __FILE__, __func__);
	#endif
	
	tetris_t* tetris = tetris_new(TETRIS_NORMAL);
	
	tetris_start(tetris);
	assert(tetris_was_started(tetris));
	assert(tetris_is_running(tetris));
	assert(!tetris_was_paused(tetris));
	
	tetris_pause(tetris);
	assert(tetris_was_started(tetris));
	assert(!tetris_is_running(tetris));
	assert(tetris_was_paused(tetris));
	
	tetris_continue(tetris);
	assert(tetris_was_started(tetris));
	assert(tetris_is_running(tetris));
	assert(!tetris_was_paused(tetris));
	
	tetris_stop(tetris);
	assert(!tetris_was_started(tetris));
	assert(!tetris_is_running(tetris));
	assert(!tetris_was_paused(tetris));
	
	tetris_free(&tetris);
	
	#if debug == 1 
		printf("<<<\n");
	#endif
}

static void _test_tetris_add_stone() {
	#if debug == 1 
		printf(">>>\n%s => %s\n", __FILE__, __func__);
	#endif
	
	tetris_t* tetris = tetris_new(TETRIS_NORMAL);
	tetris_start(tetris);
	
	tetris_create_stone_response_t stone_response;
	tetris_create_stone_random(tetris, &stone_response);
	
	assert(stone_response.type == tetris->active_stone.type);
	assert(tetris->active_stone.type > TETRIS_NO_STONE);
	assert(tetris->active_stone.type < TETRIS_MAX_STONE);
	assert(stone_response.direction == tetris->active_stone.direction);
	assert(stone_response.direction == TETRIS_NORTH);
	assert(stone_response.position.row == tetris->active_stone.position.row);
	assert(stone_response.position.row == 0);
	assert(stone_response.position.col == tetris->active_stone.position.col);
	assert(stone_response.position.col == (tetris->field_size.cols/2)-1);
	
	for(int i = 11; --i;) {
		tetris_create_stone_random(tetris, &stone_response);
		#if debug == 1
		printf("stone type response/tetris: %i / %i\n", stone_response.type, tetris->active_stone.type);
		#endif
	}
	
	tetris_create_stone(TETRIS_QUAD, tetris, &stone_response);
	assert(stone_response.type == tetris->active_stone.type);
	assert(tetris->active_stone.type == TETRIS_QUAD);
	assert(stone_response.direction == tetris->active_stone.direction);
	assert(stone_response.direction == TETRIS_NORTH);
	assert(stone_response.position.row == tetris->active_stone.position.row);
	assert(stone_response.position.row == 0);
	assert(stone_response.position.col == tetris->active_stone.position.col);
	assert(stone_response.position.col == (tetris->field_size.cols/2)-1);
	
	tetris_create_stone(TETRIS_LINE, tetris, &stone_response);
	assert(stone_response.type == TETRIS_LINE);
	
	tetris_create_stone(TETRIS_ROOF, tetris, &stone_response);
	assert(stone_response.type == TETRIS_ROOF);
	
	tetris_create_stone(TETRIS_STEP_LEFT, tetris, &stone_response);
	assert(stone_response.type == TETRIS_STEP_LEFT);
	
	tetris_create_stone(TETRIS_STEP_RIGHT, tetris, &stone_response);
	assert(stone_response.type == TETRIS_STEP_RIGHT);
	
	tetris_create_stone(TETRIS_L_LEFT, tetris, &stone_response);
	assert(stone_response.type == TETRIS_L_LEFT);
	
	tetris_create_stone(TETRIS_L_RIGHT, tetris, &stone_response);
	assert(stone_response.type == TETRIS_L_RIGHT);
	
	tetris_create_stone(-66, tetris, &stone_response);
	assert(stone_response.type == TETRIS_QUAD);
	
	tetris_create_stone(66, tetris, &stone_response);
	assert(stone_response.type == TETRIS_QUAD);
	
	tetris_free(&tetris);
	#if debug == 1 
		printf("<<<\n");
	#endif
}

#if debug

static void __print_tetris_field(tetris_t* tetris) {
	unsigned int cols = tetris->field_size.cols;
	unsigned int rows = tetris->field_size.rows;
	printf("\n");
	for ( unsigned int row = 0 ; row < rows ; ++row ) {
		for ( unsigned int col = 0 ; col < cols ; ++col ) {
			
			const stone_position_t cur_pos = tetris->active_stone.position;
			const stone_t *cur_stone = &tetris_stones[tetris->active_stone.type].stones[tetris->active_stone.direction];
			unsigned int field_value = tetris->field[row * cols + col];
			for (unsigned int cell_no = 4; cell_no--; ) {
				stone_data_t cur_cell = cur_stone->cells[cell_no];
				unsigned int new_row = cur_pos.row + cur_cell.row;
				unsigned  new_col = cur_pos.col + cur_cell.col;
				if ((row == new_row) && (col == new_col)) {
					field_value = tetris->active_stone.type;
				}
			}
			
			printf(" %i ", field_value);
		}
		printf("\n");
	}
}

static void __print_next_four_rotations(tetris_t* tetris, const char *stone_name) {	
	printf("-- Tetris start %s north --\n", stone_name);
	__print_tetris_field(tetris);
	
	tetris_move_response_t response;
	response.full_lines = NULL;
	
	tetris_rotate_stone(tetris, &response);
	
	printf("-- Tetris start %s east --\n", stone_name);
	__print_tetris_field(tetris);
	
	tetris_rotate_stone(tetris, &response);
	
	printf("-- Tetris start %s south --\n", stone_name);
	__print_tetris_field(tetris);
	
	tetris_rotate_stone(tetris, &response);
	
	printf("-- Tetris start %s west --\n", stone_name);
	__print_tetris_field(tetris);
}

static void __print_movement_(tetris_t* tetris, const char *stone_name) {	
	printf("-- Tetris start %s  --\n", stone_name);
	__print_tetris_field(tetris);
	
	tetris_move_response_t response;
	response.full_lines = NULL;
	
	printf("-- Tetris start move %s left --\n", stone_name);
	
	for ( int left = 10; left--; ) {
		tetris_move_stone_left(tetris, &response);
		__print_tetris_field(tetris);
	}
	for ( int right = 15; right--; ) {
		tetris_move_stone_right(tetris, &response);
		__print_tetris_field(tetris);
	}
	for ( int down = 19; down--; ) {
		tetris_move_stone_down(tetris, &response);
		assert(response.full_lines == NULL);
		__print_tetris_field(tetris);
	}

}

#endif

static void _test_tetris_rotate_stones() {
	#if debug == 3  
		printf(">>>\n%s => %s\n", __FILE__, __func__);
	#endif
	
	tetris_t* tetris = tetris_new(TETRIS_NORMAL);
	tetris_start(tetris);
	
	tetris_create_stone_response_t stone_response;
	tetris_create_stone(TETRIS_QUAD, tetris, &stone_response);
	
	#if debug == 3  
	__print_next_four_rotations(tetris, "Quad");
	#endif
	
	tetris_create_stone(TETRIS_LINE, tetris, &stone_response);
	
	#if debug == 3  
	__print_next_four_rotations(tetris, "Line");
	#endif
	
	tetris_create_stone(TETRIS_ROOF, tetris, &stone_response);
	
	#if debug == 3  
	__print_next_four_rotations(tetris, "Roof");
	#endif
	
	tetris_create_stone(TETRIS_STEP_LEFT, tetris, &stone_response);
	
	#if debug == 3  
	__print_next_four_rotations(tetris, "Step Left");
	#endif
	
	tetris_create_stone(TETRIS_STEP_RIGHT, tetris, &stone_response);
	
	#if debug == 3  
	__print_next_four_rotations(tetris, "Step Right");
	#endif
	
	tetris_create_stone(TETRIS_L_LEFT, tetris, &stone_response);
	
	#if debug == 3  
	__print_next_four_rotations(tetris, "L Left");
	#endif
	
	tetris_create_stone(TETRIS_L_RIGHT, tetris, &stone_response);
	
	#if debug == 3  
	__print_next_four_rotations(tetris, "L Right");
	#endif
	
	tetris_free(&tetris);
	#if debug == 3 
		printf("<<<\n");
	#endif
}

static void _test_tetris_move_stones() {
	#if debug == 2
		printf(">>>\n%s => %s\n", __FILE__, __func__);
	#endif
	
	tetris_t* tetris = tetris_new(TETRIS_NORMAL);
	tetris_start(tetris);
	
	tetris_create_stone_response_t stone_response;
	tetris_create_stone(TETRIS_QUAD, tetris, &stone_response);
	
	#if debug == 2 
	__print_movement_(tetris, "Quad");
	#endif
	
	tetris_move_response_t response;
	response.full_lines = NULL;
	
	tetris_create_stone(TETRIS_LINE, tetris, &stone_response);
	tetris_rotate_stone(tetris, &response);
	
	#if debug == 2 
	__print_movement_(tetris, "Line");
	#endif
	
	tetris_create_stone(TETRIS_ROOF, tetris, &stone_response);
	tetris_rotate_stone(tetris, &response);
	
	#if debug == 2 
	__print_movement_(tetris, "Line");
	#endif
	
	tetris_create_stone(TETRIS_STEP_LEFT, tetris, &stone_response);
	
	#if debug == 2 
	__print_movement_(tetris, "Step Left");
	#endif
	
	tetris_create_stone(TETRIS_STEP_RIGHT, tetris, &stone_response);
	
	#if debug == 2 
	__print_movement_(tetris, "Step right");
	#endif
	
	tetris_create_stone(TETRIS_L_LEFT, tetris, &stone_response);
	
	#if debug == 2 
	__print_movement_(tetris, "L Left");
	#endif
	
	tetris_create_stone(TETRIS_L_RIGHT, tetris, &stone_response);
	
	#if debug == 2 
	__print_movement_(tetris, "L right");
	#endif
	
	tetris_free(&tetris);
	#if debug == 2 
		printf("<<<\n");
	#endif
}

static void _test_tetris_remove_one_line() {
	#if debug == 4
		printf(">>>\n%s => %s\n", __FILE__, __func__);
	#endif
	
	tetris_t* tetris = tetris_new(TETRIS_NORMAL);
	tetris_start(tetris);
	
	tetris_create_stone_response_t stone_response;
	
	tetris_move_response_t response;
	response.full_lines = NULL;
	
	tetris_create_stone(TETRIS_QUAD, tetris, &stone_response);
	for ( int left = 3; left--; ) tetris_move_stone_left(tetris, &response);
	for ( int down = 19; down--; ) tetris_move_stone_down(tetris, &response);

	
	tetris_create_stone(TETRIS_QUAD, tetris, &stone_response);
	for ( int down = 19; down--; ) tetris_move_stone_down(tetris, &response);
	
	tetris_create_stone(TETRIS_QUAD, tetris, &stone_response);
	
	for ( int right = 3; right--; ) tetris_move_stone_right(tetris, &response);
	for ( int down = 19; down--; ) tetris_move_stone_down(tetris, &response);
	
	tetris_create_stone(TETRIS_LINE, tetris, &stone_response);
	tetris_rotate_stone(tetris, &response);
	for ( int left = 4; left--; ) tetris_move_stone_left(tetris, &response);
	for ( int down = 18; down--; ) tetris_move_stone_down(tetris, &response);
	
	tetris_create_stone(TETRIS_LINE, tetris, &stone_response);
	tetris_rotate_stone(tetris, &response);
	for ( int down = 18; down--; ) tetris_move_stone_down(tetris, &response);
	
	tetris_create_stone(TETRIS_QUAD, tetris, &stone_response);
	
	for ( int right = 4; right--; ) tetris_move_stone_right(tetris, &response);
	for ( int down = 15; down--; ) tetris_move_stone_down(tetris, &response);
	
	#if debug == 4
	printf("One step before closing line\n");
	__print_tetris_field(tetris);
	#endif
	
	for ( int down = 1; down--; ) tetris_move_stone_down(tetris, &response);
	
	#if debug == 4
	printf("Line closed, but not finised turn\n");
	__print_tetris_field(tetris);
	#endif
	
	for ( int down = 1; down--; ) {
		tetris_move_stone_down(tetris, &response);
		assert(response.full_lines != NULL);
		assert(response.full_lines->next == NULL);
		assert(response.full_lines->cnt == 1);
		assert(response.full_lines->lines[0] == 17);
		assert(response.full_lines->lines[1] == -1);
		assert(response.full_lines->lines[2] == -1);
		assert(response.full_lines->lines[3] == -1);
		#if debug == 4
		printf("full line %i removed\n", response.full_lines->lines[0]);
		#endif
		tetris_reset_move_response(tetris, &response);
	}
	#if debug == 4
	printf("Line closed, turn was finished based on no movement possible\nLine should be removed\n");
	__print_tetris_field(tetris);
	#endif
	
	tetris_free(&tetris);
	#if debug == 4
		printf("<<<\n");
	#endif
}

static void _test_tetris_remove_two_lines_beside() {
	#if debug == 5
		printf(">>>\n%s => %s\n", __FILE__, __func__);
	#endif
	
	tetris_t* tetris = tetris_new(TETRIS_NORMAL);
	tetris_start(tetris);
	
	tetris_create_stone_response_t stone_response;

	tetris_move_response_t response;
	response.full_lines = NULL;
	
	tetris_create_stone(TETRIS_QUAD, tetris, &stone_response);
	for ( int left = 4; left--; ) tetris_move_stone_left(tetris, &response);
	for ( int down = 19; down--; ) tetris_move_stone_down(tetris, &response);

	
	tetris_create_stone(TETRIS_QUAD, tetris, &stone_response);
	for ( int left = 2; left--; ) tetris_move_stone_left(tetris, &response);
	for ( int down = 19; down--; ) tetris_move_stone_down(tetris, &response);
	
	tetris_create_stone(TETRIS_QUAD, tetris, &stone_response);
	for ( int down = 19; down--; ) tetris_move_stone_down(tetris, &response);
	
	tetris_create_stone(TETRIS_QUAD, tetris, &stone_response);
	for ( int right = 2; right--; ) tetris_move_stone_right(tetris, &response);
	for ( int down = 19; down--; ) tetris_move_stone_down(tetris, &response);
	
	tetris_create_stone(TETRIS_QUAD, tetris, &stone_response);
	for ( int right = 4; right--; ) tetris_move_stone_right(tetris, &response);
	for ( int down = 17; down--; ) tetris_move_stone_down(tetris, &response);
	
	#if debug == 5
	printf("One step before closing line\n");
	__print_tetris_field(tetris);
	#endif
	
	for ( int down = 1; down--; ) tetris_move_stone_down(tetris, &response);
	
	#if debug == 5
	printf("Line closed, but not finised turn\n");
	__print_tetris_field(tetris);
	#endif
	
	for ( int down = 1; down--; ) {
		tetris_move_stone_down(tetris, &response);
		assert(response.full_lines != NULL);
		assert(response.full_lines->next == NULL);
		assert(response.full_lines->cnt == 2);
		assert(response.full_lines->lines[0] == 19);
		assert(response.full_lines->lines[1] == 18);
		assert(response.full_lines->lines[2] == -1);
		assert(response.full_lines->lines[3] == -1);
		#if debug == 5
		printf("full line %i and %i removed\n", response.full_lines->lines[0], response.full_lines->lines[1]);
		#endif
		tetris_reset_move_response(tetris, &response);
	}
	#if debug == 5
	printf("Line closed, turn was finished based on no movement possible\nLine should be removed\n");
	__print_tetris_field(tetris);
	#endif
	
	tetris_free(&tetris);
	#if debug == 5
		printf("<<<\n");
	#endif
}

/**
	We want to have this:
	
	 0  0  0  0  0  0  0  0  0  0
	 0  0  0  0  0  0  0  0  0  0
	 0  0  0  0  0  0  0  0  0  0
	 0  0  0  0  0  0  0  0  0  0
	 0  0  0  0  0  0  0  0  0  0
	 0  0  0  0  0  0  0  0  0  0
	 0  0  0  0  0  0  0  0  0  0
	 0  0  0  0  0  0  0  0  0  0
	 0  0  0  0  0  0  0  0  9  0
	 4  8  8  8  7  7  9  9  9  x
	 4  8  5  5  6  7  7  3  0  x
	 4  4  5  5  6  6  6  3  3  x
	 1  1  1  1  2  2  2  2  3  x
*/
static void _test_tetris_remove_three_lines_skipped() {
	#if debug == 6
		printf(">>>\n%s => %s\n", __FILE__, __func__);
	#endif
	
	tetris_t* tetris = tetris_new(TETRIS_NORMAL);
	tetris_start(tetris);
	
	tetris_create_stone_response_t stone_response;
	tetris_move_response_t response;
	response.full_lines = NULL;
	
	tetris_create_stone(TETRIS_LINE, tetris, &stone_response);
	tetris_rotate_stone(tetris, &response);
	for ( int left = 4; left--; ) tetris_move_stone_left(tetris, &response);
	for ( int down = 20; down--; ) tetris_move_stone_down(tetris, &response);
	
	tetris_create_stone(TETRIS_LINE, tetris, &stone_response);
	tetris_rotate_stone(tetris, &response);
	for ( int down = 20; down--; ) tetris_move_stone_down(tetris, &response);
	
	tetris_create_stone(TETRIS_STEP_RIGHT, tetris, &stone_response);
	tetris_rotate_stone(tetris, &response);
	for ( int right = 3; right--; ) tetris_move_stone_right(tetris, &response);
	for ( int down = 18; down--; ) tetris_move_stone_down(tetris, &response);
	
	tetris_create_stone(TETRIS_L_LEFT, tetris, &stone_response);
	tetris_rotate_stone(tetris, &response);
	tetris_rotate_stone(tetris, &response);
	for ( int left = 4; left--; ) tetris_move_stone_left(tetris, &response);
	for ( int down = 17; down--; ) tetris_move_stone_down(tetris, &response);
	
	tetris_create_stone(TETRIS_QUAD, tetris, &stone_response);
	for ( int left = 2; left--; ) tetris_move_stone_left(tetris, &response);
	for ( int down = 18; down--; ) tetris_move_stone_down(tetris, &response);
	
	tetris_create_stone(TETRIS_L_RIGHT, tetris, &stone_response);
	tetris_rotate_stone(tetris, &response);
	tetris_rotate_stone(tetris, &response);
	tetris_rotate_stone(tetris, &response);
	for ( int down = 18; down--; ) tetris_move_stone_down(tetris, &response);
	
	tetris_create_stone(TETRIS_STEP_LEFT, tetris, &stone_response);
	for ( int down = 17; down--; ) tetris_move_stone_down(tetris, &response);
	
	tetris_create_stone(TETRIS_L_LEFT, tetris, &stone_response);
	tetris_rotate_stone(tetris, &response);
	tetris_rotate_stone(tetris, &response);
	tetris_rotate_stone(tetris, &response);
	for ( int left = 3; left--; ) tetris_move_stone_left(tetris, &response);
	for ( int down = 17; down--; ) tetris_move_stone_down(tetris, &response);
	
	tetris_create_stone(TETRIS_L_LEFT, tetris, &stone_response);
	tetris_rotate_stone(tetris, &response);
	for ( int right = 2; right--; ) tetris_move_stone_right(tetris, &response);
	for ( int down = 16; down--; ) tetris_move_stone_down(tetris, &response);
	
	tetris_create_stone(TETRIS_LINE, tetris, &stone_response);
	for ( int right = 5; right--; ) tetris_move_stone_right(tetris, &response);
	for ( int down = 15; down--; ) tetris_move_stone_down(tetris, &response);
	
	#if debug == 6
	printf("One step before closing line\n");
	__print_tetris_field(tetris);
	#endif
	
	for ( int down = 1; down--; ) tetris_move_stone_down(tetris, &response);
	
	#if debug == 6
	printf("Line closed, but not finised turn\n");
	__print_tetris_field(tetris);
	#endif
	
	for ( int down = 1; down--; ) {
		tetris_move_stone_down(tetris, &response);
		assert(response.full_lines != NULL);
		assert(response.full_lines->next == NULL);
		assert(response.full_lines->cnt == 3);
		assert(response.full_lines->lines[0] == 19);
		assert(response.full_lines->lines[1] == 18);
		assert(response.full_lines->lines[2] == 16);
		assert(response.full_lines->lines[3] == -1);
		#if debug == 6
		printf("full line %i, %i and %i removed\n", response.full_lines->lines[0], response.full_lines->lines[1], response.full_lines->lines[2]);
		#endif
		tetris_reset_move_response(tetris, &response);
	}
	
	#if debug == 6
	printf("Line closed, turn was finished based on no movement possible\nLine should be removed\n");
	__print_tetris_field(tetris);
	#endif
	
	tetris_free(&tetris);
	#if debug == 6
		printf("<<<\n");
	#endif
}

int 
main() 
{
	srand(time(NULL));
	_test_tetris_alloc_();
	_test_tetris_commands();
	_test_tetris_add_stone();
	_test_tetris_rotate_stones();
	_test_tetris_move_stones();
	_test_tetris_remove_one_line();
	_test_tetris_remove_two_lines_beside();
	_test_tetris_remove_three_lines_skipped();
	return 0;
}
