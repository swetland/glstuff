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

#include "util.h"

struct v3 {
	float x;
	float y;
	float z;
};

struct v2 {
	float u;
	float v;
};

struct i3 {
	unsigned v;
	unsigned vt;
	unsigned vn;
};

struct obj {
	struct v3 *v;
	struct v3 *vn;
	struct v2 *vt;	
	struct i3 *f;
	unsigned max_v, num_v;
	unsigned max_vn, num_vn;
	unsigned max_vt, num_vt;
	unsigned max_f, num_f;
};

static void obj_destroy(struct obj *o) {
	if (o->v) free(o->v);
	if (o->vn) free(o->vn);
	if (o->vt) free(o->vt);
	if (o->f) free(o->f);
	free(o);
}

static void *resize(void *array, unsigned *max, unsigned esz) {
	void *tmp;
	unsigned n = *max ? *max * 2 : 32;
	tmp = realloc(array, esz * n);
	if (!tmp)
		free(array);
	*max = n;
	return tmp;
}
	
static int obj_add_v(struct obj *o, float x, float y, float z) {
	unsigned n = o->num_v;
	if (n == o->max_v)
		if(!(o->v = (v3*) resize(o->v, &o->max_v, sizeof(struct v3))))
			return -1;
	o->v[n].x = x;
	o->v[n].y = y;
	o->v[n].z = z;
	o->num_v = n + 1;
	return 0;		
}

static int obj_add_vn(struct obj *o, float x, float y, float z) {
	unsigned n = o->num_vn;
	if (n == o->max_vn)
		if(!(o->vn = (v3*) resize(o->vn, &o->max_vn, sizeof(struct v3))))
			return -1;
	o->vn[n].x = x;
	o->vn[n].y = y;
	o->vn[n].z = z;
	o->num_vn = n + 1;
	return 0;		
}

static int obj_add_vt(struct obj *o, float u, float v) {
	unsigned n = o->num_vt;
	if (n == o->max_vt)
		if(!(o->vt = (v2*) resize(o->vt, &o->max_vt, sizeof(struct v3))))
			return -1;
	o->vt[n].u = u;
	o->vt[n].v = v;
	o->num_vt = n + 1;
	return 0;		
}

static int obj_add_f(struct obj *o, int v, int vt, int vn) {
	unsigned n = o->num_f;
	if (n == o->max_f)
		if(!(o->f = (i3*) resize(o->f, &o->max_f, sizeof(struct v3))))
			return -1;

	/* XXX: range check */
	v -= 1;
	vt -= 1;
	vn -= 1;

	o->f[n].v = v;
	o->f[n].vt = vt;
	o->f[n].vn = vn;
	o->num_f = n + 1;
	return 0;		
}

struct obj *load_obj(const char *fn) {
	char buf[128];
	FILE *fp;
	struct obj *o = 0;
	float x, y, z;
	int a, b, c;

	if (!(fp = fopen(fn, "r")))
		goto exit;

	o = (obj*) malloc(sizeof(struct obj));
	if (!o)
		goto close_and_exit;
	memset(o, 0, sizeof(struct obj));

	while (fgets(buf, sizeof(buf), fp) != 0) {
		if (!strncmp(buf, "v ", 2)) {
			sscanf(buf + 2, "%f %f %f", &x, &y, &z);
			obj_add_v(o, x, y, z);
		} else if (!strncmp(buf, "vn ", 3)) {
			sscanf(buf + 3, "%f %f %f", &x, &y, &z);
			obj_add_vn(o, x, y, z);
		} else if (!strncmp(buf, "vt ", 3)) {
			sscanf(buf + 3, "%f %f", &x, &y);
			obj_add_vt(o, x, y);
		} else if (!strncmp(buf, "f ", 2)) {
			char *tmp = buf + 2;
			/* XXX: handle non-triangles */
			while (sscanf(tmp, "%d/%d/%d", &a, &b, &c) == 3) {
				obj_add_f(o, a, b, c);
				tmp = strstr(tmp, " ");
				if (!tmp) break;
				tmp++;
			}
		} else {
//			fprintf(stderr,"ignoring: %s", buf);
		}
	}

close_and_exit:
	fclose(fp);
exit:
	return o;	
}

static struct model *obj_to_model(struct obj *o) {
	int i, j, n;
	struct model *m;

	if(!(m = (model*) malloc(sizeof(struct model))))
		return 0;
	memset(m, 0, sizeof(struct model));

	if (!(m->vdata = (float*) malloc(sizeof(float) * 8 * o->num_f))) {
		free(m);
		return 0;
	}
	if (!(m->idx = (unsigned short *) malloc(sizeof(short) * o->num_f))) {
		free(m->vdata);
		free(m);
		return 0;
	}

	for (n = 0, i = 0; i < o->num_f; i++) {
		float data[8];
		data[0] = o->v[o->f[i].v].x;
		data[1] = o->v[o->f[i].v].y;
		data[2] = o->v[o->f[i].v].z;
		data[3] = o->vn[o->f[i].vn].x;
		data[4] = o->vn[o->f[i].vn].y;
		data[5] = o->vn[o->f[i].vn].z;
		data[6] = o->vt[o->f[i].vt].u;
		data[7] = o->vt[o->f[i].vt].v;
		for (j = 0; j < n; j++)
			if (!memcmp(data, &m->vdata[8 * j], sizeof(float) * 8))
				goto found_it;
		memcpy(&m->vdata[8 * j], data, sizeof(float) * 8);
		n++;
found_it:
		m->idx[i] = j;
	}

	m->vcount = n;
	m->icount = o->num_f;

	return m;
}

#if 0
void model_dump(struct model *m, FILE *fp) {
	int i;
	fprintf(fp, "struct model M = {\n");
	fprintf(fp, "  .vdata = {\n");
	for (i = 0; i < m->vcount; i++)
		fprintf(fp, "    %f, %f, %f,\n    %f, %f, %f, %f, %f,\n",
			m->vdata[i*8+0], m->vdata[i*8+1],
			m->vdata[i*8+2], m->vdata[i*8+3], 
			m->vdata[i*8+4], m->vdata[i*8+5],
			m->vdata[i*8+6], m->vdata[i*8+7]);
	fprintf(fp, "  },\n  .idx[] = {\n");
	for (i = 0; i < m->icount; i += 3) 
		fprintf(fp, "    %d, %d, %d,\n",
			m->idx[i+0], m->idx[i+1], m->idx[i+2]);
	fprintf(fp, "  },\n  .vcount = %d,\n  .icount = %d,\n};\n",
		m->vcount, m->icount);
}
#endif

struct model *load_wavefront_obj(const char *fn) {
	struct obj *o;
	struct model *m;
	o = load_obj(fn);
	if (!o)
		return 0;
	m = obj_to_model(o);
	obj_destroy(o);
	return m;
} 

