#ifndef VIDEO_H
#define VIDEO_H
#include <vector>
#include "window.h"

class Video{
	bool good;
	std::vector<Window> windows;
public:
	Video();
	~Video();

	// create a window and return its pointer
	Window * makeWindow(
			char const * name,
			unsigned width,
			unsigned height);

};

#endif // VIDEO_H

