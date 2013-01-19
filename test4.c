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

#include <sys/time.h>
#include <sys/stat.h>

void *texdata;
unsigned texw, texh;

char *vert_src, *frag_src;

GLuint pgm = -1, vshd = -1, fshd = -1, tex0;
GLuint aVertex, aTexCoord;
GLuint uMVP, uTexture;
 
mat4 MVP;

GLfloat verts[] = {
	-1, -1, 0,
	-1, 1, 0,
	1, -1, 0,
	1, 1, 0,
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

time_t vstime = 0, fstime = 0;

void update_shaders(void) {
	struct stat vsstat, fsstat;
	GLuint newpgm, newvs, newfs;

	stat("test4.vs", &vsstat);
	stat("test4.fs", &fsstat);

	if ((vsstat.st_mtime == vstime) && (fsstat.st_mtime == fstime))
		return;

	vstime = vsstat.st_mtime;
	fstime = fsstat.st_mtime;

	if (!(vert_src = load_file("test4.vs", 0)))
		goto exit;
	if (!(frag_src = load_file("test4.fs", 0)))
		goto free_vsrc_and_exit;

	if (shader_compile(vert_src, frag_src, &newpgm, &newvs, &newfs))
		goto free_both_and_exit;

	glDeleteShader(vshd);
	glDeleteShader(fshd);
	glDeleteProgram(pgm);

	pgm = newpgm;
	vshd = newvs;
	fshd = newfs;

	aVertex = glGetAttribLocation(pgm, "aVertex");
	aTexCoord = glGetAttribLocation(pgm, "aTexCoord");
	uMVP = glGetUniformLocation(pgm, "uMVP");
	uTexture = glGetUniformLocation(pgm, "uTexture");

	free(frag_src);
	free(vert_src);

	fprintf(stderr,"shaders updated\n");
	return;

free_both_and_exit:
	free(frag_src);
free_vsrc_and_exit:
	free(vert_src);
exit:
	fprintf(stderr,"failed to update shaders\n");
	return;
}

int scene_init(struct ctxt *c) {
	float aspect;

	if (!(texdata = load_png_gray("texture.sdf.png", &texw, &texh, 1)))
		return -1;

 	aspect = ((float)c->width) / ((float)c->height);
	mtx_ortho(MVP, -aspect, aspect, -1, 1, 1, -1);

	glViewport(0, 0, c->width, c->height);
	glClearColor(0, 0, 0, 0);
	glClearDepth(1.0f);

	glEnable(GL_TEXTURE_2D);

	glGenTextures(1, &tex0);
	glBindTexture(GL_TEXTURE_2D, tex0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, texw, texh, 0, GL_ALPHA,
		GL_UNSIGNED_BYTE, texdata);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return 0;
}

int scene_draw(struct ctxt *c) {
	update_shaders();

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

