#ifndef input_H
#define input_H
#include "SDL2/SDL.h"
#include <string>
class KeyPress{
public:
	bool faster;
	bool slower;
	bool left;
	bool right;
	bool fire;
	bool quit;
	bool any;
};
class KeyBinding{
public:
	SDL_Keycode
		faster,
		slower,
		left,
		right,
		fire;
};
void loadKeyBinding(std::string filename, KeyBinding &);
#endif
