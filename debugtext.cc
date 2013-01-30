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
#include <stdarg.h>
#include <string.h>
#ifndef _WIN32
#include <unistd.h>
#endif

#include "opengl.h"
#include "util.h"
#include "matrix.h"

#include "debugtext.h"

struct DebugTextCell {
	unsigned char x;
	unsigned char y;
	unsigned char z;
	unsigned char w;
	unsigned char u;
	unsigned char v;
	unsigned char i;
	unsigned char j;
};

static void dtc_init(DebugTextCell *data, int w, int h) {
	int x, y;
	DebugTextCell *p = data;
	memset(p, 0, sizeof(DebugTextCell) * w * h * 6);
	y = 1;
	for (y = h; y > 0; y--) {
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

int DebugText::init(unsigned w, unsigned h) {
	cbw = w;
	cbh = h;
	cx = 0;
	cy = 0;
	dirty = 0;

	data = (DebugTextCell*) malloc(sizeof(DebugTextCell) * w * h * 6);
	cbdata = (unsigned char*) malloc(sizeof(unsigned char) * w * h);

	dtc_init(data, 32, 32);

	if (!(fontdata = load_png_rgba("font-vincent-8x8.png", &fdw, &fdh, 1))) 
		return -1;

	if (pgm.compile("test2d.vs", "test2d.fs"))
		return -1;

	aVertex = pgm.getAttribID("aVertex");
	aTexCoord = pgm.getAttribID("aTexCoord");
	uMVP = pgm.getUniformID("uMVP");
	uTex0 = pgm.getUniformID("uTexture0");
	uTex1 = pgm.getUniformID("uTexture1");

	glGenTextures(1, &tex0);
	glGenTextures(1, &tex1);
	
	glBindTexture(GL_TEXTURE_2D, tex0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fdw, fdh, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, fontdata);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	memset(cbdata, 0, cbw * cbh);

	glBindTexture(GL_TEXTURE_2D, tex1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, cbw, cbh, 0, GL_ALPHA,
		GL_UNSIGNED_BYTE, cbdata);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(DebugTextCell) * cbw * cbh * 6,
		data, GL_STATIC_DRAW);

	return 0;
}

int DebugText::render(void) {
	mat4 MVP;
	MVP.setOrtho(0.0, cbw, 0.0, cbh, 1.0, -1.0);

	pgm.use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	if (dirty) {
		dirty = 0;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, cbw, cbh, 0, GL_ALPHA,
			GL_UNSIGNED_BYTE, cbdata);
	}

	glUniformMatrix4fv(uMVP, 1, GL_FALSE, MVP);
	glUniform1i(uTex0, 0);
	glUniform1i(uTex1, 1);

	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

	glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_ALPHA);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glVertexAttribPointer(aVertex, 4, GL_UNSIGNED_BYTE, GL_FALSE, 8, (void*) 0);
	glVertexAttribPointer(aTexCoord, 4, GL_UNSIGNED_BYTE, GL_FALSE, 8, (void*) 4);
	
	glEnableVertexAttribArray(aVertex);
	glEnableVertexAttribArray(aTexCoord);

	glDrawArrays(GL_TRIANGLES, 0, cbw * cbh * 6);
	glDisableVertexAttribArray(aVertex);
	glDisableVertexAttribArray(aTexCoord);

	return 0;
}

void DebugText::putch(unsigned c) {
	switch (c) {
	case 10:
		cx = 0;
		if (cy < cbh - 1)
			cy++;
		return;
	case 13:
		return;
	case 9:
		c = ' ';
		break;
	}
	cbdata[cx + cy * cbw] = c;

	if (++cx == cbw) {
		cx = 0;
		if (++cy == cbh) {
			cy = cbh - 1;
		}
	}

	dirty = 1;
}

void DebugText::puts(const char *s) {
	while (*s)
		putch(*s++);
}

void DebugText::clear(void) {
	memset(cbdata, 0, cbw * cbh);
	cx = 0;
	cy = 0;
	dirty = 1;
}

void DebugText::printf(const char *fmt, ...) {
	va_list ap;
	char tmp[128];

	va_start(ap, fmt);
	vsnprintf(tmp, sizeof(tmp), fmt, ap);
	va_end(ap);

	puts(tmp);
}

#if 0
DebugText DT;

int scene_init(struct ctxt *c) {
	glViewport(0, 0, c->width, c->height);
	glClearColor(0, 0, 180, 255);
	glClearDepth(1.0f);
	DT.init(32, 32);
	return 0;
}

int fr = 0;

int scene_draw(struct ctxt *c) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	DT.printf("Frame #%d\n", fr++);
	DT.render();
	return 0;
}
#endif
