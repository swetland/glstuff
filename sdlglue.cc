/* Copyright 2013 Brian Swetland <swetland@frotz.net>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifndef _WIN32
#include <unistd.h>
#endif

#include <SDL.h>

#ifdef _WIN32
#define GLUE_DEFINE_EXTENSIONS
#endif

#include "opengl.h"
#include "util.h"
#include "app.h"

static void die(const char *fmt, ...) {
	fprintf(stderr,"ERROR: %s\n", fmt);
	exit(-1);
}

#ifdef _WIN32
#if !WITH_SDL2
static int SDL_GL_ExtensionSupported(const char *name) {
	if (strstr((char*)glGetString(GL_EXTENSIONS), name))
		return 1;
	else
		return 0;
}
#endif

static void glsl_init(void) {
	int n;
	if (!SDL_GL_ExtensionSupported("GL_ARB_shader_objects") ||
		!SDL_GL_ExtensionSupported("GL_ARB_shading_language_100") ||
		!SDL_GL_ExtensionSupported("GL_ARB_vertex_shader") ||
		!SDL_GL_ExtensionSupported("GL_ARB_fragment_shader"))
		die("missing glsl extensions");
	for (n = 0; n < sizeof(fntb)/sizeof(fntb[0]); n++) {
		*fntb[n].func = SDL_GL_GetProcAddress(fntb[n].name);
		if (!(*fntb[n].func))
			die("cannot find func '%s'", fntb[n].name);
	}
}
#else
void glsl_init(void) {}
#endif

static void quit(void) {
	SDL_Quit();
	exit(0);
}

App::App() : _width(640), _height(480), _vsync(1), _fps(0) {
	memset(_keystate, 0, sizeof(_keystate));
}

App::~App() {
}

void App::setSize(int w, int h) {
	_width = w;
	_height = h;
}

void App::handleEvents(void) {
	SDL_Event ev;

	while (SDL_PollEvent(&ev)) {
		switch (ev.type) {
		case SDL_KEYDOWN:
			_keystate[ev.key.keysym.sym] = 1;
			if (ev.key.keysym.sym == SDLK_ESCAPE)
				quit();
			break;
		case SDL_KEYUP:
			_keystate[ev.key.keysym.sym] = 0;
			key(ev.key.keysym.sym);
			break;
		case SDL_QUIT:
			quit();
		}
	}
}

void App::setOptions(int argc, char **argv) {
	char *x;
	argc--;
	argv++;
	while (argc--) {
		if (!strcmp("-nosync",argv[0])) {
			_vsync = 0;
		} else if (isdigit(argv[0][0]) && (x = strchr(argv[0],'x'))) {
			_width = atoi(argv[0]);
			_height = atoi(x + 1);
		} else {
			fprintf(stderr,"unknown argument '%s'\n",argv[0]);
		}
		argv++;
	}
}

int App::run(void) {
	time_t t0, t1;
	int count;
#if WITH_SDL2
	SDL_Window *w;
	SDL_GLContext gc;
#endif

	if (SDL_Init(SDL_INIT_VIDEO))
		die("sdl video init failed");

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
 
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);

#if 0 
	SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE,    8);
	SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE,    8);
	SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE,    8);
	SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE,    8);
 
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS,  1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,  2);
#endif

	/* enable vsync */
	SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, _vsync);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, _vsync);

#if WITH_SDL2
	if (!(w = SDL_CreateWindow("Test", 0, 0, _width, _height,
		SDL_WINDOW_OPENGL)))
		die("sdl cannot create window");

	if (!(gc = SDL_GL_CreateContext(w)))
		die("sdl cannot create gl context");
#else
	if (SDL_SetVideoMode(_width, _height, 32,
		SDL_HWSURFACE | SDL_GL_DOUBLEBUFFER | SDL_OPENGL) == NULL) 
		die("sdl cannot set mode");
#endif	

	glsl_init();

	if (init())
		return -1;

	t0 = time(0);
	count = 0;
	for (;;) {
		handleEvents();

		if (render())
			return -1;

		if (_vsync) {
#if WITH_SDL2 
			SDL_GL_SwapWindow(w);
#else
			SDL_GL_SwapBuffers();
#endif
		} else {
			glFlush();
		}

		t1 = time(0);
		count++;
		if (t0 != t1) {
			_fps = count;
			printf("%d fps\n", count);
			count = 0;
			t0 = t1;
		}
	}
	return -1;
}

