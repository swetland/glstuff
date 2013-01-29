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

#ifndef _DEBUG_TEXT_H_
#define _DEBUG_TEXT_H_

#include "program.h"

struct DebugTextCell;

class DebugText {
private:
	unsigned char *cbdata;
	unsigned cbw, cbh;

	void *fontdata;
	unsigned fdw, fdh;

	Program pgm;

	struct DebugTextCell *data;

	GLuint tex0, tex1, vbo;
	GLuint aVertex, aTexCoord;
	GLuint uMVP, uTex0, uTex1;

	unsigned cx, cy;
	unsigned dirty;

public:
	int init(unsigned w, unsigned h);
	int render(void);

	void clear(void);
	void putch(unsigned c);
	void puts(const char *s);
	void printf(const char *fmt, ...);
};

#endif

