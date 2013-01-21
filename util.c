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
		float ai0 = a[i][0];
		float ai1 = a[i][1];
		float ai2 = a[i][2];
		float ai3 = a[i][3];
		m[i][0] = ai0*b[0][0] + ai1*b[1][0] + ai2*b[2][0] + ai3*b[3][0];
		m[i][1] = ai0*b[0][1] + ai1*b[1][1] + ai2*b[2][1] + ai3*b[3][1];
		m[i][2] = ai0*b[0][2] + ai1*b[1][2] + ai2*b[2][2] + ai3*b[3][2];
		m[i][3] = ai0*b[0][3] + ai1*b[1][3] + ai2*b[2][3] + ai3*b[3][3];
	}
}

void mtx_mul(mat4 m, mat4 a, mat4 b) {
	mat4 t;
	mtx_mul_unsafe(t, a, b);
	memcpy(m, t, sizeof(mat4));
}

void mtx_mul_vec4(vec4 o, mat4 m, vec4 v) {
	float v0=v[0], v1=v[1], v2=v[2], v3=v[3];
	o[0] = m[0][0] * v0 + m[1][0] * v1 + m[2][0] * v2 + m[3][0] * v3;
	o[1] = m[0][1] * v0 + m[1][1] * v1 + m[2][1] * v2 + m[3][1] * v3;
	o[2] = m[0][2] * v0 + m[1][2] * v1 + m[2][2] * v2 + m[3][2] * v3;
	o[3] = m[0][3] * v0 + m[1][3] * v1 + m[2][3] * v2 + m[3][3] * v3;
}

void mtx_identity(mat4 m) {
	memset(m, 0, sizeof(mat4));
	m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1.0f;
}

void mtx_translation(mat4 m, float x, float y, float z) {
	memset(m, 0, sizeof(mat4));
	m[3][0] = x;
	m[3][1] = y;
	m[3][2] = z;
	m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1.0;
}

void mtx_translate(mat4 m, float x, float y, float z) {
	mat4 t;
	mtx_translation(t, x, y, z);
	mtx_mul_unsafe(m, m, t);
}

void mtx_x_rotation(mat4 r, float angle) {
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
}

void mtx_y_rotation(mat4 r, float angle) {
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
}

void mtx_z_rotation(mat4 r, float angle) {
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
}

void mtx_rotate_x(mat4 m, float angle) {
	mat4 r;
	mtx_x_rotation(r, angle);
	mtx_mul_unsafe(m, m, r);
}

void mtx_rotate_y(mat4 m, float angle) {
	mat4 r;
	mtx_y_rotation(r, angle);
	mtx_mul_unsafe(m, m, r);
}

void mtx_rotate_z(mat4 m, float angle) {
	mat4 r;
	mtx_z_rotation(r, angle);
	mtx_mul_unsafe(m, m, r);
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

