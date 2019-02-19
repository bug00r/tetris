#ifndef TETRIS_H
#define TETRIS_H

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

/*
 Following Stones are supported
		Horizontal			Vertical
 
Quad:    ___				___
		|_|_|			   |_|_|
		|_|_|			   |_|_|

Line:   _______             _
	   |_|_|_|_|		   |_|
						   |_|
						   |_|
						   |_|


Roof:       _     _               _
		  _|_|_  |_|_   _____   _|_|
		 |_|_|_| |_|_| |_|_|_| |_|_|
                 |_|     |_|     |_|
				 
Step left:  ___       _ 
		   |_|_|_   _|_|
			 |_|_| |_|_| 
		           |_|
				   
Step right:   _ _   _ 
		    _|_|_| |_|_
		   |_|_|   |_|_|
		             |_|
					 
L left:  ___       _   _
		|_|_|  ___|_| |_|     _____
		  |_| |_|_|_| |_|_   |_|_|_|
		  |_|         |_|_|  |_|
		  
L right  ___            _   _
        |_|_| _____    |_| |_|___
        |_|  |_|_|_|  _|_| |_|_|_|
        |_|      |_| |_|_|        


*/	
typedef enum {
	TETRIS_NO_STONE		= 0,
	TETRIS_QUAD 		= 1,
	TETRIS_LINE			= 2,
	TETRIS_ROOF			= 3,
	TETRIS_STEP_LEFT	= 4,
	TETRIS_STEP_RIGHT	= 5,
	TETRIS_L_LEFT		= 6,
	TETRIS_L_RIGHT		= 7,
	TETRIS_MAX_STONE	= 8,
} stone_type_t;

typedef enum {
	TETRIS_NORTH 	= 0,
	TETRIS_EAST 	= 1,
	TETRIS_SOUTH 	= 2,
	TETRIS_WEST 	= 3,
} stone_direction_t;

typedef enum {
	TETRIS_NORMAL 	= 0,
} field_type_t;

typedef enum {
	TETRIS_RUNS 		= 0,
	TETRIS_NOT_RUNNING 	= 1,
	TETRIS_PAUSE 		= 2,
} tetris_run_state_t;

/* 
	struct which contains the cell which contains stone fragment, start 
	point is imaginary top left at 0,0 the complete roundtrip based on a 
	4*4 matrix for realisation larges stone "Line"
*/
typedef struct {
	unsigned int row;
	unsigned int col;
} stone_data_t;
typedef stone_data_t stone_position_t;

typedef struct {
	stone_data_t cells[4]; 	/* every tetris stone contains 4 cells */
} stone_t;

typedef struct {
	stone_t stones[4];		/* Every Stone has four rotation states */
} stone_collection_t;

typedef struct {
	unsigned int rows;		/* number of rows inside a tetris game field */
	unsigned int cols;		/* number of cols inside a tetris game field */
} tetris_field_size_t;

typedef struct {
	stone_type_t 		type;		/* Type of the current stone */
	stone_direction_t 	direction;	/* Direction of the current stone */
	stone_position_t  	position;	/* Position of the current game Stone */
} current_stone_t;
typedef current_stone_t tetris_create_stone_response_t;

typedef struct {
	stone_type_t		stone_type;		/* last/current stone type */
	stone_direction_t	stone_direction;/* last/current stone direction */
	unsigned int		stone_cell;		/* last/current cell index from stone */
} field_cell_t;

typedef struct {
	//unsigned int 			*field;					/* Persistent Game Field */
	field_cell_t			*field;					/* Persistent Game Field */
	tetris_field_size_t 	field_size;				/* Size of the given game Field */
	current_stone_t 		active_stone;			/* Parameter of current game Stone */
	tetris_run_state_t 		state;					/* Actual Game State */
	field_type_t 			field_type;				/* current field type */
} tetris_t;

typedef struct _tetris_full_lines_ {
	unsigned int 				cnt;		/* Number of Full Lines */
	int 						lines[4];	/* Index of the Maximum possible 4 Numbers(invalid Index = -1). */
	struct _tetris_full_lines_ 	*next;		/* Pointer to the next removed line bunch*/
} tetris_full_lines_t;

//TODO not in use yet, have to add into move_left, move_right and move_down function
typedef struct {
	current_stone_t		stone;			/* State of the current stone */
	bool 				was_moved;		/* Stone was moved */			
	bool 				was_rotated;	/* Stone was rotated */
	tetris_full_lines_t *full_lines;	/* found full lines information */
} tetris_move_response_t;

/*
	In tetris you can rotate in only one direction, in circle of clock.
	This Array can made you work easier.
		- next direction of north:
			tetris_next_direction_of[TETRIS_NORTH] == TETRIS_EAST;
			tetris_next_direction_of[TETRIS_EAST] == TETRIS_SOUTH;
			tetris_next_direction_of[TETRIS_SOUTH] == TETRIS_WEST;
			tetris_next_direction_of[TETRIS_WEST] == TETRIS_NORTH;
*/
extern const stone_direction_t tetris_next_direction_of[];

/*
	Here you can fetch wanted tetris stone with:
		tetris_stones[TETRIS_QUAD];
		tetris_stones[TETRIS_LINE];
		tetris_stones[TETRIS_ROOF];
		tetris_stones[TETRIS_STEP_LEFT];
		tetris_stones[TETRIS_STEP_RIGHT];
		tetris_stones[TETRIS_L_LEFT];
		tetris_stones[TETRIS_L_RIGHT];
	
	Additional you can fetch direction directly by:
		tetris_stones[TETRIS_QUAD].stones.[NORTH | EAST | SOUTH | WEST];
		tetris_stones[TETRIS_LINE].stones.[NORTH | EAST | SOUTH | WEST];
		tetris_stones[TETRIS_ROOF].stones.[NORTH | EAST | SOUTH | WEST];
		tetris_stones[TETRIS_STEP_LEFT].stones.[NORTH | EAST | SOUTH | WEST];
		tetris_stones[TETRIS_STEP_RIGHT].stones.[NORTH | EAST | SOUTH | WEST];
		tetris_stones[TETRIS_L_LEFT].stones.[NORTH | EAST | SOUTH | WEST];
		tetris_stones[TETRIS_L_RIGHT].stones.[NORTH | EAST | SOUTH | WEST];
*/
extern const stone_collection_t tetris_stones[];


/*
	Default invalid Tetris Stone Position
*/
extern const stone_position_t TETRIS_NO_POSITION;


/*
	This function creates a new tetris game object.
	
	Parameter		Decription
	---------		-----------------------------------------
	type			Field type, can be TETRIS_[NORMAL | LARGE]
	
	return
	------
	The new tetris game object
*/
//tetris_t* tetris_new(field_type_t type);
tetris_t* tetris_new();
/*
	This function free memory from tetris game object and set
	used pointer to NULL.
	
	Parameter		Decription
	---------		-----------------------------------------
	tetris			pointer to pointer to tetris object
*/
void tetris_free(tetris_t** tetris);

/*
	This function resets complete tetris game.
	
	Parameter		Decription
	---------		-----------------------------------------
	tetris			pointer to tetris game object
*/
void tetris_reset(tetris_t* tetris);

/*
	Starts a new tetris game.
	
	Parameter		Decription
	---------		-----------------------------------------
	tetris			pointer to tetris game object
	
*/
void tetris_start(tetris_t* tetris);
/*
	Set a break to current tetris game.
	
	Parameter		Decription
	---------		-----------------------------------------
	tetris			pointer to tetris game object
*/
void tetris_pause(tetris_t* tetris);

/* 
	This function continous current game. If no game was started there is nothing happend.
	
	Parameter		Decription
	---------		-----------------------------------------
	tetris			pointer to tetris game object
	
*/
void tetris_continue(tetris_t* tetris);

/*
	This functions stops current tetris game. You can't restart again you have to reset
	and restart then.
	
	Parameter		Decription
	---------		-----------------------------------------
	tetris			pointer to tetris game object

*/
void tetris_stop(tetris_t* tetris);

/*
	This function returns the status of pause.
	
	Parameter		Decription
	---------		-----------------------------------------
	tetris			pointer to tetris game object
	
	return
	------
	true if Tetris Game was paused, otherwise false
*/
bool tetris_was_paused(tetris_t* tetris);

/*
	This function returns the status of started.
	
	Parameter		Decription
	---------		-----------------------------------------
	tetris			pointer to tetris game object
	
	return
	------
	true if Tetris Game was started(also paused too), otherwise false
*/
bool tetris_was_started(tetris_t* tetris);

/*
	This function returns the status of runnig.
	
	Parameter		Decription
	---------		-----------------------------------------
	tetris			pointer to tetris game object
	
	return
	------
	true if Tetris Game runs, otherwise false
*/
bool tetris_is_running(tetris_t* tetris);

/*
	This function creates a new random stone and put it into game as current new.
	
	Parameter		Decription
	---------		-----------------------------------------
	tetris			pointer to tetris game object
	response		pointer to a creation response object. This
					will include all needed informations of the
					new tetris game stone.
*/
void tetris_create_stone_random(tetris_t* tetris, tetris_create_stone_response_t* response);

/*
	This function creates a new random stone and put it into game as current new.
	
	Parameter		Decription
	---------		-----------------------------------------
	stone			The type of stone which should be the next one inside of tetris game.
	tetris			pointer to tetris game object
	response		pointer to a creation response object. This
					will include all needed informations of the
					new tetris game stone.
*/
void tetris_create_stone(stone_type_t stone, tetris_t* tetris, tetris_create_stone_response_t* response);

/*
	This function rotates the current given tetris game stone.
	
	Parameter		Decription
	---------		-----------------------------------------
	tetris			pointer to tetris game object
*/
void tetris_rotate_stone(tetris_t* tetris, tetris_move_response_t * response);

/*
	This function moved the current given tetris game stone to
	the left.
	
	Parameter		Decription
	---------		-----------------------------------------
	tetris			pointer to tetris game object
	response		Pointer for storing moving response information
					response.full_lines will always be NULL
*/
void tetris_move_stone_left(tetris_t* tetris, tetris_move_response_t * response);

/*
	This function moved the current given tetris game stone to
	the right.
	
	Parameter		Decription
	---------		-----------------------------------------
	tetris			Pointer to tetris game object
	response		Pointer for storing moving response information
					response.full_lines will always be NULL
*/
void tetris_move_stone_right(tetris_t* tetris, tetris_move_response_t *response);

/*
	This function moved the current given tetris game stone down.
	
	Parameter		Decription
	---------		-----------------------------------------
	tetris			pointer to tetris game object
	response		Pointer for storing moving response information.
					Full_lines will be filled if arose, otherwise NULL.
*/
void tetris_move_stone_down(tetris_t *tetris, tetris_move_response_t *response);

/*
	This function resets the last tetris move response.
	
	Parameter		Decription
	---------		-----------------------------------------
	tetris			pointer to tetris game object
	response		Pointer for storing moving response.
*/
void tetris_reset_move_response(tetris_t *tetris ,tetris_move_response_t *response);

/*
	This function return randomized tetris stone
	
	return
	------
	new stone type
*/
stone_type_t tetris_random_stone();
#endif
