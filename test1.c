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

#include "util.h"
#include "glue.h"

void *texdata;
unsigned texw, texh;

const char *vert_src, *frag_src;

GLuint pgm, vshd, fshd, tex0;
GLuint aVertex, aTexCoord;
GLuint uMVP, uTexture;
 
mat4 MVP;

GLfloat verts[] = {
	-0.5f, -0.5f, 0.0f,
	-0.5f, 0.5f, 0.0f,
	0.5f, -0.5f, 0.0f,
	0.5f, 0.5f, 0.0f,	
};

GLfloat texcoords[] = {
	0.0, 0.0,
	0.0, 1.0,
	1.0, 0.0,
	1.0, 1.0,
};

int scene_init(struct ctxt *c) {
	if (!(texdata = load_png_rgba("texture1.png", &texw, &texh, 1))) 
		return -1;
	if (!(vert_src = load_file("test1.vs", 0)))
		return -1;
	if (!(frag_src = load_file("test1.fs", 0)))
		return -1;

	mtx_identity(MVP);
	mtx_ortho(MVP, -2.66, 2.66, -2, 2, 1, -1);

	glViewport(0, 0, c->width, c->height);
	glClearColor(0, 0, 0, 0);
	glClearDepth(1.0f);

	if (shader_compile(vert_src, frag_src, &pgm, &vshd, &fshd))
		return -1;

	aVertex = glGetAttribLocation(pgm, "aVertex");
	aTexCoord = glGetAttribLocation(pgm, "aTexCoord");
	uMVP = glGetUniformLocation(pgm, "uMVP");
	uTexture = glGetUniformLocation(pgm, "uTexture");

	if(glGetError() != GL_NO_ERROR) fprintf(stderr,"OOPS!\n");

	glEnable(GL_TEXTURE_2D);
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

int scene_draw(struct ctxt *c) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glUseProgram(pgm);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex0);

	glUniformMatrix4fv(uMVP, 1, GL_FALSE, (void*) MVP);
	glUniform1i(uTexture, 0);

	glVertexAttribPointer(aVertex, 3, GL_FLOAT, GL_FALSE, 0, verts);
	glEnableVertexAttribArray(aVertex);

	glVertexAttribPointer(aTexCoord, 2, GL_FLOAT, GL_FALSE, 0, texcoords);
	glEnableVertexAttribArray(aTexCoord);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	return 0;
}

