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

#include "app.h"
#include "util.h"
#include "matrix.h"
#include "program.h"

class TestApp : public App {
public:
	int init(void);
	int render(void);

private:
	void *texdata;
	unsigned texw, texh;

	GLuint tex0;
	GLuint aVertex, aTexCoord;
	GLuint uMVP, uTexture;
 
	mat4 MVP;
	Program pgm;
};

static GLfloat verts[] = {
	-0.5f, -0.5f, 0.0f,
	-0.5f, 0.5f, 0.0f,
	0.5f, -0.5f, 0.0f,
	0.5f, 0.5f, 0.0f,	
};

static GLfloat texcoords[] = {
	0.0, 0.0,
	0.0, 1.0,
	1.0, 0.0,
	1.0, 1.0,
};

int TestApp::init(void) {
	if (!(texdata = load_png_rgba("texture1.png", &texw, &texh, 1))) 
		return -1;

	MVP.setOrtho(-2.66, 2.66, -2, 2, 1, -1);

	glViewport(0, 0, width(), height());
	glClearColor(0, 0, 0, 0);
	glClearDepth(1.0f);

	if (pgm.compile("test1.vs","test1.fs"))
		return -1;

	aVertex = pgm.getAttribID("aVertex");
	aTexCoord = pgm.getAttribID("aTexCoord");
	uMVP = pgm.getUniformID("uMVP");
	uTexture = pgm.getUniformID("uTexture");

	if(glGetError() != GL_NO_ERROR) fprintf(stderr,"OOPS!\n");

	glEnable(GL_TEXTURE_2D);

	glGenTextures(1, &tex0);
	glBindTexture(GL_TEXTURE_2D, tex0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texw, texh, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, texdata);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	return 0;
}

int TestApp::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	pgm.use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex0);

	glUniformMatrix4fv(uMVP, 1, GL_FALSE, MVP);
	glUniform1i(uTexture, 0);

	glVertexAttribPointer(aVertex, 3, GL_FLOAT, GL_FALSE, 0, verts);
	glEnableVertexAttribArray(aVertex);

	glVertexAttribPointer(aTexCoord, 2, GL_FLOAT, GL_FALSE, 0, texcoords);
	glEnableVertexAttribArray(aTexCoord);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	return 0;
}

int main(int argc, char **argv) {
	TestApp app;
	app.setOptions(argc, argv);
	return app.run();
}
