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
GLuint _vPosition, _vUV;
GLuint _uMVP, _uSampler;

mat4 perspective;
mat4 MVP;

float a = 0.0;

struct model *m;

int scene_init(struct ctxt *c) {
	float aspect = ((float) c->width) / ((float) c->height);

	if (!(texdata = load_png_rgba("cube-texture.png", &texw, &texh))) 
		return -1;
	if (!(vert_src = load_file("test1.vertex.glsl", 0)))
		return -1;
	if (!(frag_src = load_file("test1.fragment.glsl", 0)))
		return -1;

	if (!(m = load_wavefront_obj("cube.obj")))
		return -1;

	mtx_identity(MVP);
	mtx_perspective(perspective, 60.0, aspect, 1.0, 10.0);

	glViewport(0, 0, c->width, c->height);
	glClearColor(0, 0, 0, 0);
	glClearDepth(1.0f);

	if (shader_compile(vert_src, frag_src, &pgm, &vshd, &fshd))
		return -1;

	_vPosition = glGetAttribLocation(pgm, "vPosition");
	_vUV = glGetAttribLocation(pgm, "vUV");
	_uMVP = glGetUniformLocation(pgm, "uMVP");
	_uSampler = glGetUniformLocation(pgm, "uSampler");

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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	return 0;
}

#include "data.h"

int scene_draw(struct ctxt *c) {
	mat4 camera;

	mtx_identity(camera);
	mtx_translate(camera, 0, 0, -5.0);
	mtx_rotate_y(camera, a);
	mtx_rotate_x(camera, 25.0);

	mtx_mul_unsafe(MVP, camera, perspective);

	a += 1.0;
	if (a > 360.0) a = 0.0;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(pgm);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex0);

	glUniformMatrix4fv(_uMVP, 1, GL_FALSE, (void*) MVP);
	glUniform1i(_uSampler, 0);

	glVertexAttribPointer(_vPosition, 3, GL_FLOAT, GL_FALSE, 8*4, m->vdata);
	glEnableVertexAttribArray(_vPosition);

	glVertexAttribPointer(_vUV, 2, GL_FLOAT, GL_FALSE, 8*4, m->vdata + 6);
	glEnableVertexAttribArray(_vUV);

	glDrawElements(GL_TRIANGLES, m->icount, GL_UNSIGNED_SHORT, m->idx);

	return 0;
}

