
SDLCFG := sdl-config
#SDLCFG := /work/sdl/bin/sdl2-config

SDLFLAGS := $(shell $(SDLCFG) --cflags)
SDLLIBS := $(shell $(SDLCFG) --libs)

CFLAGS := $(SDLFLAGS) -DWITH_SDL2=0 -Wall
LIBS := $(SDLLIBS) -lGL -lm -lpng

COMMONOBJS := util.o sdlglue.o loadpng.o loadfile.o

all: test1 test2

TEST1OBJS := test1.o $(COMMONOBJS)

test1: $(TEST1OBJS)
	$(CC) -o test1 $(TEST1OBJS) $(LIBS)

TEST2OBJS := test2.o $(COMMONOBJS)

test2: $(TEST2OBJS)
	$(CC) -o test2 $(TEST2OBJS) $(LIBS)

clean::
	rm -f test1 test2 *.o
