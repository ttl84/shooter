#ifndef IO_HPP
#define IO_HPP
#include "video/video.h"
#include "audio/audio.h"
#include "keyboard/keyboard.h"
struct IO {
	Video video;
	Audio audio;
	Keyboard keyboard;

	Window * window;
};
#endif // IO_HPP

