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

#ifndef _APP_H_
#define _APP_H_

#include "opengl.h"

class App {
public:
	App();
	virtual ~App();

	// configuration options
	void setSize(int w, int h);

	// parse commandline for configuration overrides
	void setOptions(int argc, char **argv);

	// state query
	int fps(void) { return _fps; };
	int width(void) { return _width; };
	int height(void) { return _height; };
	float aspect(void) { return ((float) _width) / ((float) _height); };
	int keydown(int code) { return _keystate[code]; }

	int run(void);

	virtual int init(void) = 0;
	virtual int render(void) = 0;

private:
	int _width, _height;
	int _vsync;
	int _fps;

	char _keystate[512];
	void handleEvents(void);
};

#endif
