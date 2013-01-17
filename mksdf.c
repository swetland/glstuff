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

#include <math.h>

#include "util.h"

unsigned char *tex;
unsigned tw, th;

unsigned sample(int x, int y) {
	if (x < 0) x = 0;
	if (x >= tw) x = tw-1;
	if (y < 0) y = 0;
	if (y > th) y = th-1;
	return tex[x + y * tw] >> 7;
}

/* this is absurdly brute-force and clunky */
unsigned distance(int cx, int cy, int d) {
	int x, y;
	float dn = d*d+1.0, t; 
	unsigned cs = sample(cx, cy);
	unsigned r;

	for (y = cy - d; y <= cy + d; y++) {
		for (x = cx - d; x <= cx + d; x++) {
			if (sample(x, y) != cs) {
				t = (cx-x)*(cx-x)+(cy-y)*(cy-y);
				if (t < dn) dn = t;
			}
		}
	}

	dn = sqrt(dn);
	r = ((127.0 * dn) / ((float) d));
	if (r > 127) r = 127;
	if (cs) 
		return 127 - r;
	else
		return 127 + r;
}

/* for each texel in the output texture, find the distance from its
 * corresponding texel in the input texture to the nearest pixel of
 * the opposite color 
 */
void generate(unsigned char *map, int mw, int mh, int d) {
	int x, y;
	int up = tw / mw;
	int dn = up / 2;
	for (y = 0; y < mh; y++)
		for (x = 0; x < mh; x++)
			map[y*mw+x] = distance(x * up + dn, y * up + dn, d);
}

int main(int argc, char **argv) {
	unsigned char *map;
	unsigned mw, mh;
	int x, y;

	if (argc != 4) {
		fprintf(stderr,"usage: mksdf <pngfile> <size> <outfile>\n");
		return -1;
	}

	mw = mh = atoi(argv[2]);

	if (!(map = malloc(mw * mh))) {
		fprintf(stderr,"out of memory\n");
		return -1;
	}

	tex = load_png_gray(argv[1], &tw, &th, 0);
	if (!tex) {
		fprintf(stderr,"cannot load source image '%s'\n", argv[1]);
		return -1;
	}

	generate(map, mw, mh, tw / mw);

	return save_png_gray(argv[3], map, mw, mh);
}

