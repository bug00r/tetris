#ifndef TETRIS_DEFS_H
#define TETRIS_DEFS_H

#ifndef SDL_RECT_LOC
	#define SDL_RECT_LOC(var,_x,_y,_w,_h) \
				SDL_Rect var;	\
				rect.x = (_x);	\
				rect.y = (_y);	\
				rect.w = (_w);	\
				rect.h = (_h)
#endif

#ifndef EXTERN_BLOB
	#define EXTERN_BLOB(name,suffix) \
		extern unsigned char _binary_ ## name ## _ ## suffix ## _start; \
		extern unsigned char _binary_ ## name ## _ ## suffix ## _size
#endif

#define UNUSED(x) (void)(x)

#endif