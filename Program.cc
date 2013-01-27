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

#include "glue.h"
#include "util.h"
#include "Program.h"

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

