MAKE?=mingw32-make
AR?=ar
ARFLAGS?=rcs
PATHSEP?=/
CC=gcc
BUILDROOT?=build

ifeq ($(CLANG),1)
	export CC=clang
endif

BUILDDIR?=$(BUILDROOT)$(PATHSEP)$(CC)
BUILDPATH?=$(BUILDDIR)$(PATHSEP)

ifdef DEBUG
	export debug=-Ddebug=$(DEBUG) -ggdb
	export isdebug=1
endif

ifeq ($(ANALYSIS),1)
	export analysis=-Danalysis=1
	export isanalysis=1
endif

ifeq ($(OUTPUT),1)
	export outimg= -Doutput=1
endif

CFLAGS=-std=c11 -Wpedantic -pedantic-errors -Wall -Wextra -ggdb -O1 
#-ggdb -O1
#-pg for profiling 

INCLUDEDIR= -I./../_third_/SDL2_env/SDL2_build_msys2/include \
			-I.
		   
SRC=tetris.c

LIBNAME=libtetris.a
OBJS=tetris.o

TESTSRC=test_tetris.c
TESTBIN=test_tetris.exe
TESTLIB=-ltetris
TESTLIBDIR=-L$(BUILDPATH)

UI_SRC=main.c tetris_gfx.c tetris_sfx.c tetris_ui.c tetris_input.c tetris_game.c tetris_app.c
UI_BIN=tetris.exe
UI_LIB=-ltetris -lmingw32 -lSDL2main -lSDL2 -static -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lvorbisfile -lvorbis -lvorbisenc -logg -lfreetype6 -lpng -lz -lm \
	   -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm \
	   -limm32 -lole32 -loleaut32 -lshell32 -lversion -luuid -static-libgcc -mwindows
#-mwindows
UI_LIBDIR=-L$(BUILDPATH) \
		  -L./../_third_/SDL2_env/SDL2_build_msys2/lib \
		  -L./../_third_/zlib-1.2.11

UI_FONT=font.o
UI_SFX=sfx.o
UI_SFX2=sfx2.o
UI_SFX3=sfx3.o
UI_SFX4=sfx4.o
UI_TEXTURE=texture.o

all: createdir $(BUILDPATH)$(LIBNAME) $(BUILDPATH)$(UI_FONT) $(BUILDPATH)$(UI_SFX) $(BUILDPATH)$(UI_SFX2) \
			   $(BUILDPATH)$(UI_SFX3) $(BUILDPATH)$(UI_SFX4) $(BUILDPATH)$(UI_TEXTURE) $(BUILDPATH)$(TESTBIN) $(BUILDPATH)$(UI_BIN)

$(BUILDPATH)$(LIBNAME): $(BUILDPATH)$(OBJS)
	$(AR) $(ARFLAGS) $(BUILDPATH)$(LIBNAME) $(BUILDPATH)$(OBJS)

$(BUILDPATH)$(OBJS): tetris.h
	$(CC) $(CFLAGS) -c $(SRC) -o $(BUILDPATH)$(OBJS) $(debug)
	
$(BUILDPATH)$(TESTBIN):
	$(CC) $(CFLAGS) $(TESTSRC) -o $(BUILDPATH)$(TESTBIN) $(TESTLIBDIR) $(TESTLIB) $(debug)
	
$(BUILDPATH)$(UI_BIN):
	$(CC) $(CFLAGS) $(UI_SRC) -o $(BUILDPATH)$(UI_BIN) $(INCLUDEDIR)  $(BUILDPATH)$(UI_FONT) $(BUILDPATH)$(UI_SFX) $(BUILDPATH)$(UI_SFX2) $(BUILDPATH)$(UI_SFX3) $(BUILDPATH)$(UI_SFX4) $(BUILDPATH)$(UI_TEXTURE) $(UI_LIBDIR) $(UI_LIB) $(debug)

$(BUILDPATH)$(UI_FONT):
	cp ./../_third_/fonts/tele-marines/Telev2b.ttf $(BUILDPATH)tetris.ttf
	cd $(BUILDPATH); \
	ld -r -b binary tetris.ttf -o $(UI_FONT) 

$(BUILDPATH)$(UI_SFX):
	cp ./../_third_/sounds/a01.ogg $(BUILDPATH)tetris.ogg
	cd $(BUILDPATH); \
	ld -r -b binary tetris.ogg -o $(UI_SFX)

$(BUILDPATH)$(UI_SFX2):
	#cp ./../_third_/sounds/Powerup2.ogg $(BUILDPATH)tetris2.ogg
	cp ./../_third_/sounds/rotate.wav $(BUILDPATH)tetris2.ogg
	cd $(BUILDPATH); \
	ld -r -b binary tetris2.ogg -o $(UI_SFX2)
	
$(BUILDPATH)$(UI_SFX3):
	cp ./../_third_/sounds/Randomize18.ogg $(BUILDPATH)tetris3.ogg
	cd $(BUILDPATH); \
	ld -r -b binary tetris3.ogg -o $(UI_SFX3)

$(BUILDPATH)$(UI_SFX4):
	cp ./../_third_/sounds/Randomize12.ogg $(BUILDPATH)tetris4.ogg
	cd $(BUILDPATH); \
	ld -r -b binary tetris4.ogg -o $(UI_SFX4)

$(BUILDPATH)$(UI_TEXTURE):
	cp ./texture1.png $(BUILDPATH)texture.png
	cd $(BUILDPATH); \
	ld -r -b binary texture.png -o $(UI_TEXTURE)
	
.PHONY: clean createdir test run small

test:
	./$(BUILDPATH)$(TESTBIN)

run:
	./$(BUILDPATH)$(UI_BIN)
	
small:
	-strip $(BUILDPATH)$(UI_BIN)
	-strip $(BUILDPATH)$(TESTBIN)
	
createdir:
	mkdir -p $(BUILDDIR)

clean:
	-rm -dr $(BUILDROOT)
	