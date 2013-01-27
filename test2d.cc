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
#include "matrix.h"

#define USE_VBO 1
#define CDATA_RGBA 0

void *texdata;
unsigned texw, texh;

const char *vert_src, *frag_src;

GLuint pgm, vshd, fshd, tex0, tex1;
GLuint aVertex, aTexCoord;
GLuint uMVP, uTexture0, uTexture1;

#if CDATA_RGBA
unsigned cbdata[32*32];
#else
unsigned char cbdata[32*32];
#endif

unsigned cbw = 32;
unsigned cbh = 32;

struct acell {
	unsigned char x;
	unsigned char y;
	unsigned char z;
	unsigned char w;
	unsigned char u;
	unsigned char v;
	unsigned char i;
	unsigned char j;
};

struct acell adata[32 * 32 * 6];

void adata_init(struct acell *data, int w, int h) {
	int x, y;
	struct acell *p = data;
	memset(p, 0, sizeof(struct acell) * w * h * 6);
	y = 1;
	for (y = h; y > 0; y--) {
			fprintf(stderr,"%d\n",h-y);
		for (x = 0; x < w; x++) {
			p->x = x+0; p->y = y+0;
			p->u = 0;   p->v = 0;   p->i = x; p->j = h-y; p++;
			p->x = x+0; p->y = y-1;
			p->u = 0;   p->v = 1;   p->i = x; p->j = h-y; p++;
			p->x = x+1; p->y = y-1;
			p->u = 1;   p->v = 1;   p->i = x; p->j = h-y; p++;
			p->x = x+1; p->y = y-1;
			p->u = 1;   p->v = 1;   p->i = x; p->j = h-y; p++;
			p->x = x+1; p->y = y+0;
			p->u = 1;   p->v = 0;   p->i = x; p->j = h-y; p++;
			p->x = x+0; p->y = y+0;
			p->u = 0;   p->v = 0;   p->i = x; p->j = h-y; p++;
		}
	}
}
	
int scene_init(struct ctxt *c) {

	adata_init(adata, 32, 32);

	if (!(texdata = load_png_rgba("font-vincent-8x8.png", &texw, &texh, 1))) 
		return -1;
	if (!(vert_src = (const char*) load_file("test2d.vs", 0)))
		return -1;
	if (!(frag_src = (const char*) load_file("test2d.fs", 0)))
		return -1;

	glViewport(0, 0, c->width, c->height);
	glClearColor(0, 0, 180, 255);
	glClearDepth(1.0f);

	if (shader_compile(vert_src, frag_src, &pgm, &vshd, &fshd))
		return -1;

	aVertex = glGetAttribLocation(pgm, "aVertex");
	aTexCoord = glGetAttribLocation(pgm, "aTexCoord");
	uMVP = glGetUniformLocation(pgm, "uMVP");
	uTexture0 = glGetUniformLocation(pgm, "uTexture0");
	uTexture1 = glGetUniformLocation(pgm, "uTexture1");

	glGenTextures(1, &tex0);
	glGenTextures(1, &tex1);
	
	glBindTexture(GL_TEXTURE_2D, tex0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texw, texh, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, texdata);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	memset(cbdata, 0, sizeof(cbdata));
	for (int z = 0; z < 256; z++) cbdata[z] = z;
	//sprintf((char*) cbdata, "0123456789 Hello, GL World!");
	//sprintf(((char*) cbdata) + 32, "Line Number Two");
	//sprintf(((char*) cbdata) + 64, "<<-- 33333");

	glBindTexture(GL_TEXTURE_2D, tex1);
#if CDATA_RGBA
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, cbw, cbh, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, cbdata);
#else
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, cbw, cbh, 0, GL_ALPHA,
		GL_UNSIGNED_BYTE, cbdata);
#endif
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

#if USE_VBO
	GLuint buf;
	glGenBuffers(1,&buf);
	glBindBuffer(GL_ARRAY_BUFFER, buf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(adata), adata, GL_STATIC_DRAW);
#endif	
	return 0;

}

int scene_draw(struct ctxt *c) {
	mat4 MVP;

	MVP.setOrtho(0.0, 32.0, 0.0, 32.0, 1.0, -1.0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(pgm);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glUniformMatrix4fv(uMVP, 1, GL_FALSE, MVP);
	glUniform1i(uTexture0, 0);
	glUniform1i(uTexture1, 1);

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
#if USE_VBO 
	glVertexAttribPointer(aVertex, 4, GL_UNSIGNED_BYTE, GL_FALSE, 8, (void*) 0);
	glVertexAttribPointer(aTexCoord, 4, GL_UNSIGNED_BYTE, GL_FALSE, 8, (void*) 4);
#else
	glVertexAttribPointer(aVertex, 4, GL_UNSIGNED_BYTE, GL_FALSE, 8, adata);
	glVertexAttribPointer(aTexCoord, 4, GL_UNSIGNED_BYTE, GL_FALSE, 8, ((char*)adata) + 4);
#endif
	glEnableVertexAttribArray(aVertex);
	glEnableVertexAttribArray(aTexCoord);

	glDrawArrays(GL_TRIANGLES, 0, 32 * 32 * 6);

	return 0;
}

