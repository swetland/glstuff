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

#include "app.h"
#include "util.h"
#include "program.h"

static int check_compile_error(GLuint obj, const char *fn) {
	GLint r, len;
	char *buf;

	if (fn) glGetShaderiv(obj, GL_COMPILE_STATUS, &r);
	else glGetProgramiv(obj, GL_LINK_STATUS, &r);

	if (r) return 0;

	if (fn) glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &len);
	else glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &len);

	buf = (char*) malloc(len + 1);
	memset(buf, 0, len);
	if (buf != 0) {
		if (fn) glGetShaderInfoLog(obj, len, &len, buf);
		else glGetProgramInfoLog(obj, len, &len, buf);
		buf[len] = 0;
	}
	fprintf(stderr,"Shader %s Error:\n%s\n",
		fn ? "Compile" : "Link", buf ? buf : "???");

	free(buf);
	return -1;
}

static int shader_compile(const char *vshader, const char *fshader,
		GLuint *_pgm, GLuint *_vshd, GLuint *_fshd) {
	GLuint pgm, vshd, fshd;

	pgm = glCreateProgram();
	vshd = glCreateShader(GL_VERTEX_SHADER);
	fshd = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(vshd, 1, &vshader, NULL);
	glShaderSource(fshd, 1, &fshader, NULL);
	glCompileShader(vshd);
	check_compile_error(vshd, vshader);

	glCompileShader(fshd);
	check_compile_error(fshd, fshader);

	glAttachShader(pgm, vshd);
	glAttachShader(pgm, fshd);
	glLinkProgram(pgm);
	if (check_compile_error(pgm, 0))
		return -1;

	*_pgm = pgm;
	*_vshd = vshd;
	*_fshd = fshd;
	return 0;
}

Program::Program() : vsrc(NULL), fsrc(NULL), status(-1) {
}

int Program::compile(const char *vfn, const char *ffn) {
	if (!(vsrc = (char*) load_file(vfn, 0)))
		return -1;
	if (!(fsrc = (char*) load_file(ffn, 0)))
		return -1;
	if (shader_compile(vsrc, fsrc, &pobj, &vobj, &fobj))
		return -1;
	status = 0;
	return 0;
}

