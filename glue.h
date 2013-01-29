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

#ifndef _SDL_GLUE_H_
#define _SDL_GLUE_H_

#ifndef _WIN32
#define GL_GLEXT_PROTOTYPES 1
#endif

#include <SDL_opengl.h>

struct ctxt {
	unsigned width;
	unsigned height;
	void *data;
};

int scene_init(struct ctxt *c);
int scene_draw(struct ctxt *c);

int shader_compile(const char *vshader, const char *fshader,
		GLuint *pgm, GLuint *vshd, GLuint *fshd);

#ifdef _WIN32
#ifndef GLUE_DEFINE_EXTENSIONS
#define GLXTN extern
#else
#define GLXTN
#endif

GLXTN PFNGLACTIVETEXTUREPROC glActiveTexture;
GLXTN PFNGLATTACHSHADERPROC glAttachShader;
GLXTN PFNGLBINDBUFFERPROC glBindBuffer;
GLXTN PFNGLBUFFERDATAPROC glBufferData;
GLXTN PFNGLCOMPILESHADERPROC glCompileShader;
GLXTN PFNGLCREATEPROGRAMPROC glCreateProgram;
GLXTN PFNGLCREATESHADERPROC glCreateShader;
GLXTN PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
GLXTN PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
GLXTN PFNGLGENBUFFERSPROC glGenBuffers;
GLXTN PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;
GLXTN PFNGLGETPROGRAMIVPROC glGetProgramiv;
GLXTN PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
GLXTN PFNGLGETSHADERIVPROC glGetShaderiv;
GLXTN PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
GLXTN PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
GLXTN PFNGLLINKPROGRAMPROC glLinkProgram;
GLXTN PFNGLSHADERSOURCEPROC glShaderSource;
GLXTN PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
GLXTN PFNGLUNIFORM1IPROC glUniform1i;
GLXTN PFNGLUNIFORM4FVPROC glUniform4fv;
GLXTN PFNGLUSEPROGRAMPROC glUseProgram;
GLXTN PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;

#ifdef GLUE_DEFINE_EXTENSIONS
#define EFUNC(n) { (void**) &n, #n }
struct {
	void **func;
	const char *name;
} fntb[] = {
	EFUNC(glActiveTexture), EFUNC(glAttachShader), EFUNC(glBindBuffer), EFUNC(glBufferData),
	EFUNC(glCompileShader), EFUNC(glCreateProgram), EFUNC(glCreateShader), EFUNC(glDisableVertexAttribArray),
	EFUNC(glEnableVertexAttribArray), EFUNC(glGenBuffers), EFUNC(glGetAttribLocation), EFUNC(glGetProgramiv),
	EFUNC(glGetProgramInfoLog), EFUNC(glGetShaderiv), EFUNC(glGetShaderInfoLog),
	EFUNC(glGetUniformLocation), EFUNC(glLinkProgram), EFUNC(glShaderSource),
	EFUNC(glUniformMatrix4fv), EFUNC(glUseProgram), EFUNC(glUniform1i), EFUNC(glUniform4fv), EFUNC(glVertexAttribPointer),
};
#endif

#endif
#endif 
