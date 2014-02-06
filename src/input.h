#ifndef input_H
#define input_H
namespace control{
	extern bool faster;
	extern bool slower;
	extern bool left;
	extern bool right;
	extern bool fire;
	extern bool quit;
}

void handle_event(void);
#endif