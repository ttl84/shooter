#ifndef KEYBOARD_H
#define KEYBOARD_H
#include "SDL2/SDL_keycode.h"

struct KeyPress{
	bool faster;
	bool slower;
	bool left;
	bool right;
	bool shoot;
	bool quit;
	bool reset;
	bool any;
};

struct KeyBinding{
	SDL_Keycode
		reset,
		faster,
		slower,
		left,
		right,
		shoot;

	void load(char const * path);
};

struct Keyboard{
	KeyPress keyPress;
	KeyBinding keyBinding;
	Keyboard();
	~Keyboard();
	void update();
};



#endif
