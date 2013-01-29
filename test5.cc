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
#include <string.h>
#ifndef _WIN32
#include <unistd.h>
#endif

#include "util.h"
#include "matrix.h"
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
#include <SDL_keysym.h>

mat4 Projection;

float a = 0.0;

struct model *m;

int scene_init(struct ctxt *c) {
	float aspect = ((float) c->width) / ((float) c->height);

	if (!(texdata = load_png_rgba("cube-texture.png", &texw, &texh, 1)))
		return -1;
	if (!(vert_src = (const char*) load_file("test5.vs", 0)))
		return -1;
	if (!(frag_src = (const char*) load_file("test5.fs", 0)))
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

	Projection.setPerspective(D2R(60.0), aspect, 1.0, 100.0);

	return 0;
}

int scene_draw(struct ctxt *c) {
	mat4 MVP;
	mat4 MV;
	mat4 Model;
	mat4 View;
	vec4 light(0.0,0.0,0.0,1.0);

	float vz = cosf(D2R(camry));
	float vx = -sinf(D2R(camry));
	float vz2 = cosf(D2R(camry + 90.0));
	float vx2 = -sinf(D2R(camry + 90.0));

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

	View.identity().translate(-camx, camy, camz)
		.rotateY(D2R(camry)).rotateX(D2R(camrx));
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

	Model.identity().translate(20, 40, 30);
	light = Model * light;
	light = View * light;
	glUniform4fv(uLight, 1, light);

	Model.identity();
	MV.mul(Model,View);
	MVP.mul(MV,Projection);

	glUniformMatrix4fv(uMV, 1, GL_FALSE, MV);
	glUniformMatrix4fv(uMVP, 1, GL_FALSE, MVP);
	glDrawElements(GL_TRIANGLES, m->icount, GL_UNSIGNED_SHORT, m->idx);

	Model.identity().translate(-3, 0, 0);
	MV.mul(Model,View);
	MVP.mul(MV,Projection);

	glUniformMatrix4fv(uMV, 1, GL_FALSE, MV);
	glUniformMatrix4fv(uMVP, 1, GL_FALSE, MVP);
	glDrawElements(GL_TRIANGLES, m->icount, GL_UNSIGNED_SHORT, m->idx);

	Model.identity().translate(3, 0, 0).rotateY(D2R(a));
	MV.mul(Model,View);
	MVP.mul(MV,Projection);

	glUniformMatrix4fv(uMV, 1, GL_FALSE, MV);
	glUniformMatrix4fv(uMVP, 1, GL_FALSE, MVP);
	glDrawElements(GL_TRIANGLES, m->icount, GL_UNSIGNED_SHORT, m->idx);

	return 0;
}

