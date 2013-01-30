#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "app.h"
#include "util.h"
#include "matrix.h"
#include "program.h"
#include "debugtext.h"

#include <math.h>

#include <SDL_keysym.h>

class NoiseApp : public App {
public:
	NoiseApp();
	int init(void);
	int render(void);
private:
	unsigned char *data;
	int w, h;

	Program pgm;

	GLuint tex0;
	GLuint aVertex, aTexCoord;
	GLuint uTexture;

	float n;

	DebugText dtxt;

	float scale;
	float offx,offy;
};

NoiseApp::NoiseApp() : App(), w(512), h(512) {
	data = (unsigned char*) malloc(w * h);	
	scale = 1.0;
	offx = offy = 0;
	n = 0;
}

static const char *vsrc = R"(
attribute vec2 aVertex;
attribute vec2 aTexCoord;
varying vec2 vTexCoord;
void main() {
	gl_Position = vec4(aVertex, 0.0, 1.0);
	vTexCoord = aTexCoord;
}
)";

static const char *fsrc = R"(
uniform sampler2D uTexture;
varying vec2 vTexCoord;
void main() {
	float c = texture2D(uTexture, vTexCoord).a;
	gl_FragColor = vec4(c, c, c, 1.0);
}
)";

int NoiseApp::init(void) {
	if (pgm.compileStr(vsrc, fsrc))
		return -1;

	aVertex = pgm.getAttribID("aVertex");
	aTexCoord = pgm.getAttribID("aTexCoord");
	uTexture = pgm.getUniformID("uTexture");
	glViewport(0, 0, width(), height());
	glClearColor(0, 0, 0, 0);
	glClearDepth(1.0f);
	
	glGenTextures(1, &tex0);

	dtxt.init(32, 32);
	return 0;
}

static GLfloat vtx_xy_uv[] = {
	-1.0, -1.0, 0.0, 0.0,
	-1.0,  1.0, 0.0, 1.0,
	 1.0, -1.0, 1.0, 0.0,
	 1.0,  1.0, 1.0, 1.0,
};

int NoiseApp::render(void) {
	int x, y;
//	n += 0.01;

	if (keydown(SDLK_w)) offy += 0.1;
	if (keydown(SDLK_s)) offy -= 0.1;
	if (keydown(SDLK_a)) offx -= 0.1;
	if (keydown(SDLK_d)) offx += 0.1;

	if (keydown(SDLK_r)) scale += 0.1;
	if (keydown(SDLK_f)) scale -= 0.1;

	for (y = 0; y < h; y++) {
		for (x = 0; x < w; x++) {
			float fy = float(y) / float(h) + offy;
			float fx = float(x) / float(w) + offx;

	fx *= scale;
	fy *= scale;
	
			float sn =
	snoise(fx, fy) * 1.0
	+ snoise(fx*2.0,fy*6.0) * 0.25
	+ snoise(fx*24.0,fy*4.0) * 0.125 // 0.0625
#if 0 
	+ snoise(fx*8.0,fy*16.0) * 0.0625 // 0.015625
#endif
	;

#if 1
	if (sn < -1.0) sn = -1.0;
	if (sn > 1.0) sn = 1.0;
#endif

			data[y*w+x] = 127.0 * (sn + 1.0);
#if 0
			if ((sn + (1.0-fy) * 1.0 ) > 0.75)
				data[y*w+x] = 0xff;
			else
				data[y*w+x] = 0x00;
#endif
		}
	}

	glClear(GL_COLOR_BUFFER_BIT);

	pgm.use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, w, h, 0, GL_ALPHA, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glUniform1i(uTexture, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glVertexAttribPointer(aVertex, 2, GL_FLOAT, GL_FALSE, 4*4, vtx_xy_uv);
	glVertexAttribPointer(aTexCoord, 2, GL_FLOAT, GL_FALSE, 4*4, vtx_xy_uv + 2);

	glEnableVertexAttribArray(aVertex);
	glEnableVertexAttribArray(aTexCoord);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glDisableVertexAttribArray(aVertex);
	glDisableVertexAttribArray(aTexCoord);

	dtxt.clear();
	//dtxt.printf("NoiseToy v0");
	dtxt.printf("%f %f %f\n", offx, offy, scale);
	dtxt.render();
	return 0;
}

int main(int argc, char **argv) {
	NoiseApp app;
	app.setSize(512, 512);
	app.setOptions(argc, argv);
	return app.run();
}
