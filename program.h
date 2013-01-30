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

#ifndef _PROGRAM_H_
#define _PROGRAM_H_

class Program {
	GLuint pobj, vobj, fobj;
	const char *vsrc, *fsrc;
	int status;

public:
	Program();

	int compile(const char *vfn, const char *ffn);
	int compileStr(const char *vsc, const char *fsc);

	int ready() { return status == 0; }

	void use(void) {
		glUseProgram(pobj);
	}
	unsigned getAttribID(const char *name) {
		return glGetAttribLocation(pobj, name);
	}
	unsigned getUniformID(const char *name) {
		return glGetUniformLocation(pobj, name);
	}
};

#endif
