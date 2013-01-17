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

#ifndef _UTIL_H_
#define _UTIL_H_

typedef float mat4[4][4];
typedef float vec4[4];

/* load the identity matrix */
void mtx_identity(mat4 out);

/* out = left * right */
void mtx_mul(mat4 out, mat4 left, mat4 right);

/* avoids a copy, but out and left may not be the same */
void mtx_mul_unsafe(mat4 out, mat4 left, mat4 right);

/* applies the transform to out */
void mtx_translate(mat4 out, float x, float y, float z);
void mtx_rotate_x(mat4 out, float angle);
void mtx_rotate_y(mat4 out, float angle);
void mtx_rotate_z(mat4 out, float angle);

/* GLU-style perspective matrix helper functions */
void mtx_ortho(mat4 out,
	float left, float right,
	float bottom, float top,
	float near, float far);

void mtx_frustum(mat4 out,
	float left, float right, float bottom, float top,
	float znear, float zfar);

void mtx_perspective(mat4 out,
	float fov_degrees, float aspect_ratio,
	float znear, float zfar);

/* file io helpers */
void *load_png_rgba(const char *fn, unsigned *width, unsigned *height, int texture);
void *load_png_gray(const char *fn, unsigned *width, unsigned *height, int texture);
void *load_file(const char *fn, unsigned *sz);

int save_png_rgba(const char *fn, void *data, unsigned width, unsigned height);
int save_png_gray(const char *fn, void *data, unsigned width, unsigned height);

/* model helpers */

struct model {
	float *vdata; /* { vertex[3], normal[3], texcoord[2] } * vcount */
	unsigned short *idx;
	unsigned vcount;
	unsigned icount;
};

struct model *load_wavefront_obj(const char *fn);
#endif

