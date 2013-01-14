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

#include <SDL.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "util.h"
#include "glue.h"

void die(const char *fmt, ...) {
	fprintf(stderr,"ERROR: %s\n", fmt);
	exit(-1);
}

int shader_compile(const char *vshader, const char *fshader,
		GLuint *_pgm, GLuint *_vshd, GLuint *_fshd) {
	GLint r;
	GLuint pgm, vshd, fshd;

	pgm = glCreateProgram();
	vshd = glCreateShader(GL_VERTEX_SHADER);
	fshd = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(vshd, 1, &vshader, NULL);
	glShaderSource(fshd, 1, &fshader, NULL);
	glCompileShader(vshd);
	glCompileShader(fshd);
	glAttachShader(pgm, vshd);
	glAttachShader(pgm, fshd);
//	glBindAttribLocation(pgm, 0, "vPosition");
	glLinkProgram(pgm);

	glGetProgramiv(pgm, GL_LINK_STATUS, &r);

	if (!r) {
		GLint len;
		char *buf;
		glGetProgramiv(pgm, GL_INFO_LOG_LENGTH, &len);
		buf = malloc(len + 1);
		memset(buf, 0, len);
		if (buf != 0) {
			glGetProgramInfoLog(pgm, len, &len, buf);
			buf[len] = 0;
			fprintf(stderr,"<< Shader Compiler Error >>\n%s\n",buf);
			free(buf);
		}
		return -1;
	} else {
		*_pgm = pgm;
		*_vshd = vshd;
		*_fshd = fshd;
		return 0;
	}
}

void handle_events(void) {
	SDL_Event ev;

	while (SDL_PollEvent(&ev)) {
		switch (ev.type) {
		case SDL_KEYDOWN:
		case SDL_QUIT:
			SDL_Quit();
			exit(0);
		}
	}
}

int main(int argc, char **argv) {
	struct ctxt c;
#if WITH_SDL2
	SDL_Window *w;
	SDL_GLContext gc;
#endif

	c.width = 640;
	c.height = 480;
	c.data = 0;

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

#if WITH_SDL2
	if (!(w = SDL_CreateWindow("Test", 0, 0, c.width, c.height,
		SDL_WINDOW_OPENGL)))
		die("sdl cannot create window");

	if (!(gc = SDL_GL_CreateContext(w)))
		die("sdl cannot create gl context");
#else
	if (SDL_SetVideoMode(c.width, c.height, 32,
		SDL_HWSURFACE | SDL_GL_DOUBLEBUFFER | SDL_OPENGL) == NULL) 
		die("sdl cannot set mode");
#endif	

	if (scene_init(&c))
		return -1;

	for (;;) {
		handle_events();

		if (scene_draw(&c))
			return -1;

#if WITH_SDL2 
		SDL_GL_SwapWindow(w);
#else
		SDL_GL_SwapBuffers();
#endif
	}

	return 0;
}

