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
#include "program.h"
#include "matrix.h"

#include <math.h>

void *texdata;
unsigned texw, texh;

GLuint tex0;
GLuint aVertex, aTexCoord;
GLuint uMVP, uTexture;

Program pgm;
mat4 perspective;
mat4 MVP;

float a = 0.0;

GLfloat data[] = {
	-0.5f, 0.5f, 0.5f, 0.25, 0.75,
	-0.5f, -0.5f, 0.5f, 0.25, 0.50,
	0.5f, -0.5f, 0.5f, 0.50, 0.50,
	0.5f, 0.5f, 0.5f, 0.50, 0.75,

	0.5f, 0.5f, 0.5f, 0.50, 0.75,
	0.5f, -0.5f, 0.5f, 0.50, 0.50,
	0.5f, -0.5f, -0.5f, 0.75, 0.50,
	0.5f, 0.5f, -0.5f, 0.75, 0.75,

	0.5f, 0.5f, -0.5f, 0.75, 0.75,
	0.5f, -0.5f, -0.5f, 0.75, 0.50,
	-0.5f, -0.5f, -0.5f, 1.00, 0.50,
	-0.5f, 0.5f, -0.5f, 1.00, 0.75,

	-0.5f, 0.5f, -0.5f, 0.00, 0.75,
	-0.5f, -0.5f, -0.5f, 0.00, 0.50,
	-0.5f, -0.5f, 0.5f, 0.25, 0.50,
	-0.5f, 0.5f, 0.5f, 0.25, 0.75,

	-0.5f, 0.5f, -0.5f, 0.25, 1.00,
	-0.5f, 0.5f, 0.5f, 0.25, 0.75,
	0.5f, 0.5f, 0.5f, 0.50, 0.75,
	0.5f, 0.5f, -0.5f, 0.50, 1.00,

	-0.5f, -0.5f, 0.5f, 0.25, 0.50,
	-0.5f, -0.5f, -0.5f, 0.25, 0.25,
	0.5f, -0.5f, -0.5f, 0.50, 0.25,
	0.5f, -0.5f, 0.5f, 0.50, 0.50,
};

GLubyte indices[] = {
	0, 1, 2, 3, 0, 2,
	4, 5, 6, 7, 4, 6,
	8, 9, 10, 11, 8, 10,
	12, 13, 14, 15, 12, 14,
	16, 17, 18, 19, 16, 18,
	20, 21, 22, 23, 20, 22,
};

int scene_init(struct ctxt *c) {
	float aspect = ((float) c->width) / ((float) c->height);

	if (!(texdata = load_png_rgba("cube-texture.png", &texw, &texh, 1)))
		return -1;

	perspective.setPerspective(D2R(60.0), aspect, 1.0, 10.0);

	glViewport(0, 0, c->width, c->height);
	glClearColor(0, 0, 0, 0);
	glClearDepth(1.0f);

	if (pgm.compile("test1.vs", "test1.fs"))
		return -1;

	aVertex = pgm.getAttribID("aVertex");
	aTexCoord = pgm.getAttribID("aTexCoord");
	uMVP = pgm.getUniformID("uMVP");
	uTexture = pgm.getUniformID("uTexture");

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	glGenTextures(1, &tex0);
	
	glBindTexture(GL_TEXTURE_2D, tex0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texw, texh, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, texdata);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	return 0;
}

int scene_draw(struct ctxt *c) {
	mat4 camera;

	camera.identity();
	camera.rotateY(D2R(a));
	camera.rotateX(D2R(25.0));
	camera.translate(0, 0, -3.0);

	MVP = camera * perspective;

	a += 1.0;
	if (a > 360.0) a = 0.0;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	pgm.use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex0);

	glUniformMatrix4fv(uMVP, 1, GL_FALSE, MVP);
	glUniform1i(uTexture, 0);

	glVertexAttribPointer(aVertex, 3, GL_FLOAT, GL_FALSE, 5*4, data);
	glEnableVertexAttribArray(aVertex);

	glVertexAttribPointer(aTexCoord, 2, GL_FLOAT, GL_FALSE, 5*4, data + 3);
	glEnableVertexAttribArray(aTexCoord);

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, indices);

	return 0;
}

