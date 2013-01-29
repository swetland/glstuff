
SDLCFG := sdl-config
#SDLCFG := /work/sdl/bin/sdl2-config

SDLFLAGS := $(shell $(SDLCFG) --cflags)
SDLLIBS := $(shell $(SDLCFG) --libs)

CFLAGS := $(SDLFLAGS) -DWITH_SDL2=0 -Wall -g -O2
CXXFLAGS := $(CFLAGS)
LIBS := $(SDLLIBS) -lGL -lm -lpng

COMMONOBJS := sdlglue.o loadpng.o loadfile.o loadobj.o program.o matrix.o

all:: everything

APPS := test1 test2 test3 test4 test5 test2d 

define build-test
$1: $1.o $(COMMONOBJS)
	$(CC) -o $1 $1.o $(COMMONOBJS) $(LIBS)
endef

$(foreach t,$(APPS),$(eval $(call build-test,$t)))

mksdf: mksdf.o loadpng.o savepng.o
	$(CC) -o mksdf mksdf.o loadpng.o savepng.o $(LIBS)

everything: $(APPS) mksdf

clean::
	rm -f $(APPS) mksdf *.o
