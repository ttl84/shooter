#include "Sound.h"
#include <vector>
#include <atomic>
#include <iostream>
void Sound::callback(void *userdata, Uint8 *stream, int len)
{
	auto playbacks = (PlaybackList *) userdata;
	SDL_memset(stream, 0, len);
	playbacks->lock();
	for(Playback & p : playbacks->playbacks)
	{
		if(p.buf == nullptr)
			continue;
		unsigned left = p.pos + len > p.len ?  p.len - p.pos : len;
		SDL_MixAudioFormat(stream, p.buf + p.pos, playbacks->format, left, SDL_MIX_MAXVOLUME);
		
		p.pos += left;
		if(p.pos == p.len)
			p = Playback{nullptr, 0, 0};
	}
	playbacks->unlock();
}
Sound::Sound(std::string path, SDL_AudioDeviceID d)
:	buf(nullptr, [](Uint8* ptr){SDL_FreeWAV(ptr);}),
	dev(d)
{
	Uint8* tmpbuf;
	good = SDL_LoadWAV(path.c_str(), &spec, &tmpbuf, &len) != nullptr;
	buf.reset(tmpbuf);
	
}
Sound::~Sound()
{
}
Playback Sound::play()
{
	return Playback{buf.get(), len, 0};
}
bool Sound::getSpec(std::string path, SDL_AudioSpec * spec)
{
	unsigned len;
	Uint8 * buf;
	bool good = SDL_LoadWAV(path.c_str(), spec, &buf, &len);
	if(good)
		SDL_FreeWAV(buf);
	return good;
}