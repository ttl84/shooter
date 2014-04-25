#include "SDL2/SDL.h"
#include <string>
#include <memory>
struct Playback{
	Uint8 * buf;
	unsigned len;
	unsigned pos;
};
class Sound{
	std::unique_ptr<Uint8, std::function<void(Uint8*)> > buf; // buffer containing our audio file
	unsigned len; // length of our sample
	SDL_AudioSpec spec; // the specs of our piece of music
	SDL_AudioDeviceID dev;

	bool good;

	
public:
	Sound(std::string path, SDL_AudioDeviceID d);
	~Sound();
	Sound const & operator=(Sound const &) = delete;
	Sound(Sound const &) = delete;
	Playback play();

	static bool getSpec(std::string path, SDL_AudioSpec *);
	static void callback(void *userdata, Uint8 *stream, int len);
};


struct PlaybackList{
	Playback playbacks[100];
	SDL_SpinLock locked;
	SDL_AudioFormat format;
	PlaybackList()
	{
		for(auto & p : playbacks)
			p = Playback{nullptr, 0, 0};
		locked = 0;
	}
	void play(Sound *s)
	{
		lock();
		for(auto & p : playbacks)
			if(p.pos == p.len)
			{
				p = s->play();
				break;
			}
		unlock();
	}
	void lock()
	{
		SDL_AtomicLock(&locked);
	}
	void unlock()
	{
		SDL_AtomicUnlock(&locked);
	}
};
