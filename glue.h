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

#ifndef _SDL_GLUE_H_
#define _SDL_GLUE_H_

#define GL_GLEXT_PROTOTYPES 1

#include <SDL_opengl.h>

struct ctxt {
	unsigned width;
	unsigned height;
	void *data;
};

int scene_init(struct ctxt *c);
int scene_draw(struct ctxt *c);

int shader_compile(const char *vshader, const char *fshader,
		GLuint *pgm, GLuint *vshd, GLuint *fshd);

#endif 
