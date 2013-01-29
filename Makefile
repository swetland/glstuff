
SDLCFG := sdl-config
#SDLCFG := /work/sdl/bin/sdl2-config

SDLFLAGS := $(shell $(SDLCFG) --cflags)
SDLLIBS := $(shell $(SDLCFG) --libs)

CFLAGS := $(SDLFLAGS) -DWITH_SDL2=0 -Wall -g -O2
CFLAGS += -ffunction-sections -fdata-sections
LFLAGS := -Wl,-gc-sections -static-libstdc++
CXXFLAGS := $(CFLAGS)
LIBS := stuff.a $(SDLLIBS) -lGL -lm -lpng

all:: everything

LIBOBJS := loadfile.o
LIBOBJS += loadpng.o savepng.o
LIBOBJS += loadobj.o
LIBOBJS += matrix.o program.o
LIBOBJS += debugtext.o
LIBOBJS += sdlglue.o

stuff.a: $(LIBOBJS)
	rm -f stuff.a
	ar cr stuff.a $(LIBOBJS)

APPS := test1 test2 test3 test4 test5 test2d 

define build-test
$1: $1.o stuff.a
	$(CXX) $(LFLAGS) -o $1 $1.o $(COMMONOBJS) $(LIBS)
endef

$(foreach t,$(APPS),$(eval $(call build-test,$t)))

mksdf: mksdf.o loadpng.o savepng.o
	$(CXX) -o mksdf mksdf.o loadpng.o savepng.o $(LIBS)

everything: $(APPS) mksdf

clean::
	rm -f $(APPS) mksdf *.o stuff.a
