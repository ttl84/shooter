#include "SDL2/SDL.h"
#include <string>
#include <memory>
class Sound{
	std::unique_ptr<Uint8, std::function<void(Uint8*)> > buf; // buffer containing our audio file
	unsigned len; // length of our sample
	SDL_AudioSpec spec; // the specs of our piece of music
	SDL_AudioDeviceID dev;

	bool good;

	struct Playback{
		unsigned pos;
	} playback;
	static void callback(void *userdata, Uint8 *stream, int len);
public:
	Sound(std::string path);
	~Sound();
	Sound const & operator=(Sound const &) = delete;
	Sound(Sound const &) = delete;
	
	void play();
	void stop();
	
};
