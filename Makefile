
SDLCFG := sdl-config
#SDLCFG := /work/sdl/bin/sdl2-config

SDLFLAGS := $(shell $(SDLCFG) --cflags)
SDLLIBS := $(shell $(SDLCFG) --libs)

CFLAGS := $(SDLFLAGS) -DWITH_SDL2=0 -Wall -g -O2
CXXFLAGS := $(CFLAGS)
LIBS := $(SDLLIBS) -lGL -lm -lpng

COMMONOBJS := util.o sdlglue.o loadpng.o loadfile.o loadobj.o

all: test1 test2 test3 test4 test5 mksdf

mksdf: mksdf.c loadpng.c savepng.c
	gcc -g -Wall -o mksdf mksdf.c loadpng.c savepng.c -lm -lpng

TEST1OBJS := test1.o $(COMMONOBJS)
test1: $(TEST1OBJS)
	$(CC) -o test1 $(TEST1OBJS) $(LIBS)

TEST2OBJS := test2.o $(COMMONOBJS)
test2: $(TEST2OBJS)
	$(CC) -o test2 $(TEST2OBJS) $(LIBS)

TEST3OBJS := test3.o $(COMMONOBJS)
test3: $(TEST3OBJS)
	$(CC) -o test3 $(TEST3OBJS) $(LIBS)

TEST4OBJS := test4.o $(COMMONOBJS)
test4: $(TEST4OBJS)
	$(CC) -o test4 $(TEST4OBJS) $(LIBS)

TEST5OBJS := test5.o $(COMMONOBJS) matrix.o
test5: $(TEST5OBJS)
	$(CC) -o test5 $(TEST5OBJS) $(LIBS)

clean::
	rm -f test1 test2 test3 test4 test5 mksdf *.o
