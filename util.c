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

#include <math.h>
#include <string.h>

#include "util.h"

void mtx_mul_unsafe(mat4 m, mat4 a, mat4 b) {
	int i;
	for (i = 0; i < 4; i++) {
		m[i][0] =	(a[i][0] * b[0][0]) +
				(a[i][1] * b[1][0]) +
				(a[i][2] * b[2][0]) +
				(a[i][3] * b[3][0]);
		m[i][1] =	(a[i][0] * b[0][1]) +
				(a[i][1] * b[1][1]) +
				(a[i][2] * b[2][1]) +
				(a[i][3] * b[3][1]);
		m[i][2] =	(a[i][0] * b[0][2]) +
				(a[i][1] * b[1][2]) +
				(a[i][2] * b[2][2]) +
				(a[i][3] * b[3][2]);
		m[i][3] =	(a[i][0] * b[0][3]) +
				(a[i][1] * b[1][3]) +
				(a[i][2] * b[2][3]) +
				(a[i][3] * b[3][3]);
	}
}

void mtx_mul(mat4 m, mat4 a, mat4 b) {
	mat4 t;
	mtx_mul_unsafe(t, a, b);
	memcpy(m, t, sizeof(mat4));
}

void mtx_mul_vec4(vec4 o, mat4 m, vec4 v) {
	float a, b, c, d;
	a = m[0][0] * v[0] + m[1][0] * v[1] + m[2][0] * v[2] + m[3][0] * v[3];
	b = m[0][1] * v[0] + m[1][1] * v[1] + m[2][1] * v[2] + m[3][1] * v[3];
	c = m[0][2] * v[0] + m[1][2] * v[1] + m[2][2] * v[2] + m[3][2] * v[3];
	d = m[0][3] * v[0] + m[1][3] * v[1] + m[2][3] * v[2] + m[3][3] * v[3];
	o[0] = a;
	o[1] = b;
	o[2] = c;
	o[3] = d;
}

void mtx_identity(mat4 m) {
	memset(m, 0, sizeof(mat4));
	m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1.0f;
}

void mtx_translate(mat4 m, float x, float y, float z) {
	m[3][0] += (m[0][0] * x + m[1][0] * y + m[2][0] * z);
	m[3][1] += (m[0][1] * x + m[1][1] * y + m[2][1] * z);
	m[3][2] += (m[0][2] * x + m[1][2] * y + m[2][2] * z);
	m[3][3] += (m[0][3] * x + m[1][3] * y + m[2][3] * z);
}

void mtx_rotate_x(mat4 m, float angle) {
	mat4 r;
	float sa,ca;
	angle = angle * M_PI / 180.0;
	sa = sinf(angle);
	ca = cosf(angle);
	memset(r, 0, sizeof(mat4));
	r[0][0] = 1;
	r[1][1] = ca;
	r[1][2] = -sa;
	r[2][1] = sa;
	r[2][2] = ca;
	r[3][3] = 1;
	mtx_mul(m, r, m);
}

void mtx_rotate_y(mat4 m, float angle) {
	mat4 r;
	float sa,ca;
	angle = angle * M_PI / 180.0;
	sa = sinf(angle);
	ca = cosf(angle);
	memset(r, 0, sizeof(mat4));
	r[0][0] = ca;
	r[0][2] = sa;
	r[1][1] = 1;
	r[2][0] = -sa;
	r[2][2] = ca;
	r[3][3] = 1;
	mtx_mul(m, r, m);
}

void mtx_rotate_z(mat4 m, float angle) {
	mat4 r;
	float sa,ca;
	angle = angle * M_PI / 180.0;
	sa = sinf(angle);
	ca = cosf(angle);
	memset(r, 0, sizeof(mat4));
	r[0][0] = ca;
	r[0][1] = -sa;
	r[1][0] = sa;
	r[1][1] = ca;
	r[2][2] = 1;
	r[3][3] = 1;
	mtx_mul(m, r, m);
}

void mtx_frustum(mat4 m, float left, float right, float bottom, float top, float znear, float zfar) {
	float znear2 = 2.0 * znear;
	float width = right - left;
	float height = top - bottom;
	float depth = zfar - znear; 
	memset(m, 0, sizeof(mat4));
	m[0][0] = znear2 / width;
	m[1][1] = znear2 / height;
	m[2][0] = (right + left) / width;
	m[2][1] = (top + bottom) / height;
	m[2][2] = (-zfar - znear) / depth;
	m[2][3] = -1.0;
	m[3][2] = (-znear2 * zfar) / depth;
}

void mtx_perspective(mat4 m, float fov, float aspect, float znear, float zfar) {
	float ymax, xmax;
	ymax = znear * tanf(fov * M_PI / 360.0);
	xmax = ymax * aspect;
	mtx_frustum(m, -xmax, xmax, -ymax, ymax, znear, zfar);
}

void mtx_ortho(mat4 m, float l, float r, float b, float t, float n, float f) {
	memset(m, 0, sizeof(mat4));
	m[0][0] = 2.0 / (r - l);
	m[1][1] = 2.0 / (t - b);
	m[2][2] = -2.0 / (f - n);
	m[3][0] = -((r + l) / (r - l));
	m[3][1] = -((t + b) / (t - b));
	m[3][2] = -((f + n) / (f - n));
	m[3][3] = 1.0;
}

