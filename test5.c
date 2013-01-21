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
#include <unistd.h>
#include <string.h>

#include "util.h"
#include "glue.h"

#include <math.h>

void *texdata;
unsigned texw, texh;

const char *vert_src, *frag_src;

GLuint pgm, vshd, fshd, tex0;
GLuint aVertex, aNormal, aTexCoord;
GLuint uMV, uMVP, uLight, uTexture;

float camx = 0, camy = 0, camz = -5;
float camrx = 0, camry = 0, camrz = 0;

extern unsigned char keystate[];
#include <SDL/SDL_keysym.h>

mat4 Projection;

float a = 0.0;

struct model *m;

int scene_init(struct ctxt *c) {
	float aspect = ((float) c->width) / ((float) c->height);

	if (!(texdata = load_png_rgba("cube-texture.png", &texw, &texh, 1)))
		return -1;
	if (!(vert_src = load_file("test5.vs", 0)))
		return -1;
	if (!(frag_src = load_file("test5.fs", 0)))
		return -1;

	if (!(m = load_wavefront_obj("cube.obj")))
		return -1;

	glViewport(0, 0, c->width, c->height);
	glClearColor(0, 0, 0, 0);
	glClearDepth(1.0f);

	if (shader_compile(vert_src, frag_src, &pgm, &vshd, &fshd))
		return -1;

	aVertex = glGetAttribLocation(pgm, "aVertex");
	aNormal = glGetAttribLocation(pgm, "aNormal");
	aTexCoord = glGetAttribLocation(pgm, "aTexCoord");
	uMVP = glGetUniformLocation(pgm, "uMVP");
	uMV = glGetUniformLocation(pgm, "uMV");
	uLight = glGetUniformLocation(pgm, "uLight");
	uTexture = glGetUniformLocation(pgm, "uTexture");

	if(glGetError() != GL_NO_ERROR) fprintf(stderr,"OOPS!\n");

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
//	glEnable(GL_BLEND);
//	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	glGenTextures(1, &tex0);
	
	glBindTexture(GL_TEXTURE_2D, tex0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texw, texh, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, texdata);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	mtx_perspective(Projection, 60.0, aspect, 1.0, 100.0);

	return 0;
}

int scene_draw(struct ctxt *c) {
	mat4 MVP;
	mat4 MV;
	mat4 Model;
	mat4 View;
	mat4 tmp;
	vec4 light = { 0.0, 0.0, 0.0, 1.0 };

	float vz = cosf(camry * M_PI / 180.0);
	float vx = -sinf(camry * M_PI / 180.0);
	float vz2 = cosf((camry + 90.0) * M_PI / 180.0);
	float vx2 = -sinf((camry + 90.0) * M_PI / 180.0);

	if (keystate[SDLK_w]) { camx += vx * 0.1; camz += vz * 0.1; }
	if (keystate[SDLK_s]) { camx -= vx * 0.1; camz -= vz * 0.1; }
	if (keystate[SDLK_a]) { camx += vx2 * 0.1; camz += vz2 * 0.1; }
	if (keystate[SDLK_d]) { camx -= vx2 * 0.1; camz -= vz2 * 0.1; }

	if (keystate[SDLK_q]) camry += 3.0;
	if (keystate[SDLK_e]) camry -= 3.0;

	if (keystate[SDLK_r]) camrx -= 1.0;
	if (keystate[SDLK_f]) camrx += 1.0;

	if (keystate[SDLK_x]) { camrx = 0; camrz = 0; }

	if (camrx < -45.0) camrx = -45.0;
	if (camrx > 45.0) camrx = 45.0;

	mtx_identity(View);
	mtx_translation(tmp, -camx, camy, camz);
	mtx_mul(View, View, tmp);
	//mtx_translate(View, -camx, camy, camz);
	mtx_y_rotation(tmp, camry);
	mtx_mul(View, View, tmp);
	mtx_x_rotation(tmp, camrx);
	mtx_mul(View, View, tmp);

	a += 1.0;
	if (a > 360.0) a = 0.0;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(pgm);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex0);

	glUniform1i(uTexture, 0);

	glVertexAttribPointer(aVertex, 3, GL_FLOAT, GL_FALSE, 8*4, m->vdata);
	glEnableVertexAttribArray(aVertex);
	glVertexAttribPointer(aNormal, 3, GL_FLOAT, GL_FALSE, 8*4, m->vdata + 3);
	glEnableVertexAttribArray(aNormal);
	glVertexAttribPointer(aTexCoord, 2, GL_FLOAT, GL_FALSE, 8*4, m->vdata + 6);
	glEnableVertexAttribArray(aTexCoord);

	mtx_identity(Model);
	mtx_translate(Model, 20, 40, 30);
	mtx_mul_vec4(light, Model, light);
	mtx_mul_vec4(light, View, light);
	glUniform4fv(uLight, 1, light);

	mtx_identity(Model);
	mtx_mul_unsafe(MV, Model, View);
	mtx_mul_unsafe(MVP, MV, Projection);

	glUniformMatrix4fv(uMV, 1, GL_FALSE, (void*) MV);
	glUniformMatrix4fv(uMVP, 1, GL_FALSE, (void*) MVP);
	glDrawElements(GL_TRIANGLES, m->icount, GL_UNSIGNED_SHORT, m->idx);

	mtx_identity(Model);
	mtx_translate(Model, -3, 0, 0);
	mtx_mul_unsafe(MV, Model, View);
	mtx_mul_unsafe(MVP, MV, Projection);

	glUniformMatrix4fv(uMV, 1, GL_FALSE, (void*) MV);
	glUniformMatrix4fv(uMVP, 1, GL_FALSE, (void*) MVP);
	glDrawElements(GL_TRIANGLES, m->icount, GL_UNSIGNED_SHORT, m->idx);

	mtx_identity(Model);
	mtx_y_rotation(tmp, a);
	mtx_mul_unsafe(Model, Model, tmp);
	mtx_translation(tmp, 3, 0, 0);
	mtx_mul_unsafe(Model, Model, tmp);
	//mtx_translate(Model, 3, 0, 0);
	//mtx_mul(Model, tmp, Model);
	//mtx_rotate_y(Model, a);
	mtx_mul_unsafe(MV, Model, View);
	mtx_mul_unsafe(MVP, MV, Projection);

	glUniformMatrix4fv(uMV, 1, GL_FALSE, (void*) MV);
	glUniformMatrix4fv(uMVP, 1, GL_FALSE, (void*) MVP);
	glDrawElements(GL_TRIANGLES, m->icount, GL_UNSIGNED_SHORT, m->idx);

	return 0;
}

