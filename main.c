#include "tetris_app.h"

int
main(int argc, char **argv)
{

	tetris_app_t app;
	
	if(tetris_app_init(&app, argc, argv)) {
		
		tetris_app_run(&app);
	
	}
	
	tetris_app_cleanup(&app);
	
    return 0;
}