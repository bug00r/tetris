#include "tetris_gfx.h"

EXTERN_BLOB(tetris, ttf);
EXTERN_BLOB(texture, png);

static bool __tetris_gfx_init_fonts(tetris_gfx_t *gfx) {
	
	SDL_RWops* font_mem = SDL_RWFromMem(&_binary_tetris_ttf_start, (intptr_t)&_binary_tetris_ttf_size);
    if (font_mem == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Load font from memory fail : %s\n",SDL_GetError());
        return false;
    }
	
	gfx->headline_font = TTF_OpenFontRW(font_mem, 0, 36);
	if(!gfx->headline_font) {
		printf("Font opening error : %s\n",TTF_GetError());
		return false;
	}
	
	gfx->score_font = TTF_OpenFontRW(font_mem, 0, 18);
	if(!gfx->score_font) {
		printf("Font opening score font : %s\n",TTF_GetError());
		return false;
	}
	
	gfx->level_font = TTF_OpenFontRW(font_mem, 0, 18);
	if(!gfx->level_font) {
		printf("Font opening score font : %s\n",TTF_GetError());
		return false;
	}
	
	return true;
}

static bool __tetris_gfx_init_texture(tetris_gfx_t *gfx) {
	SDL_RWops *tex_mem = SDL_RWFromMem(&_binary_texture_png_start, (intptr_t)&_binary_texture_png_size);
    if (tex_mem == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Load font from memory fail : %s\n",SDL_GetError());
        return false;
    }
	
	gfx->texture = IMG_LoadPNG_RW(tex_mem);
	if(!gfx->texture) {
		printf("IMG_LoadPNG_RW: %s\n", IMG_GetError());
		return false;
	}
	
	SDL_SetSurfaceBlendMode(gfx->texture, SDL_BLENDMODE_BLEND);
	
	return true;
}

bool tetris_gfx_init(tetris_gfx_t *gfx) {
	bool init_success = false;
	if (gfx != NULL) {
		gfx->color_headline.r = 64;
		gfx->color_headline.g = 64;
		gfx->color_headline.b = 64;
		gfx->color_label.r = 64;
		gfx->color_label.g = 64;
		gfx->color_label.b = 64;
		gfx->color_value.r = 255;
		gfx->color_value.g = 255;
		gfx->color_value.b = 255;
		init_success = __tetris_gfx_init_fonts(gfx);
		init_success &= __tetris_gfx_init_texture(gfx);
	}
	return init_success;
}

static void __draw_sdl_rect_ouline_rgba(SDL_Renderer *renderer, SDL_Rect *rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	SDL_SetRenderDrawColor(renderer, r, g, b, a);
	SDL_RenderDrawRect(renderer, rect);
}

void tetris_gfx_draw_rect_outline_rgba(SDL_Renderer *renderer, int x, int y, int width, int height, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	SDL_RECT_LOC(rect,x,y,width,height);
	__draw_sdl_rect_ouline_rgba(renderer, &rect, r, g, b, a);
}

static void __draw_sdl_rect_rgba(SDL_Renderer *renderer, SDL_Rect *rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	SDL_SetRenderDrawColor(renderer, r, g, b, a);
	SDL_RenderFillRect(renderer, rect);
}

void tetris_gfx_draw_rect_rgba(SDL_Renderer *renderer, int x, int y, int width, int height, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	SDL_RECT_LOC(rect,x,y,width,height);
	__draw_sdl_rect_rgba(renderer, &rect, r, g, b, a);
}

static char * num_to_string(unsigned int number) {
	int buffsize = vsnprintf(NULL, 0, "%d", (char *)&number);
	buffsize += 1;
	char * buffer = malloc(buffsize);
	vsnprintf(buffer, buffsize, "%d", (char *)&number);
	return buffer;
}

SDL_Surface * tetris_gfx_create_text(const char *text, TTF_Font *font, SDL_Color *color) {
	SDL_Surface *text_surface = NULL;
	if(!(text_surface=TTF_RenderText_Solid(font ,text, *color))) {
		printf("Error drawing text : %s\n", TTF_GetError());
	} 
	return text_surface;
}

SDL_Surface * tetris_gfx_create_uint_text(unsigned int number, TTF_Font *font, SDL_Color *color) {
	char * num_str = num_to_string(number);
	SDL_Surface * text =  tetris_gfx_create_text(num_str ,font, color);
	free(num_str);
	return text;
}

void tetris_gfx_draw_text_at_position(SDL_Surface *text, SDL_Surface *text_target, int x, int y) {
	SDL_RECT_LOC(rect,x,y,text->w,text->h);
	SDL_BlitSurface(text, NULL, text_target, &rect);
}