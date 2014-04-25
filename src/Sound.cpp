#include "Sound.h"
void Sound::callback(void *userdata, Uint8 *stream, int len)
{
	Sound * s = (Sound*)userdata;
	if (s->playback.pos >= s->len)
	{
		s->stop();
		return;
	}
	
	if(s->playback.pos + len > s->len)
		len = s->len - s->playback.pos;
	SDL_memcpy (stream, s->buf.get() + s->playback.pos, len); 					// simply copy from one buffer into the other
	//SDL_memset(stream, 0, len);
	//SDL_MixAudioFormat(stream, s->buf.get() + s->playback.pos, s->spec.format, len, SDL_MIX_MAXVOLUME / 2);// mix from one buffer into another
	
	s->playback.pos += len;
}
Sound::Sound(std::string path):buf(nullptr, [](Uint8* ptr){SDL_FreeWAV(ptr);})
{
	Uint8* tmpbuf;
	good = SDL_LoadWAV(path.c_str(), &spec, &tmpbuf, &len) != nullptr;
	buf.reset(tmpbuf);
	
	spec.callback = Sound::callback;
	spec.userdata = this;
	
	dev = SDL_OpenAudioDevice(nullptr, 0, &spec, nullptr, SDL_AUDIO_ALLOW_ANY_CHANGE);
}
Sound::~Sound()
{
	SDL_PauseAudioDevice(dev, 1);
	SDL_CloseAudioDevice(dev);
}
void Sound::play()
{
	if(not good or dev == 0)
		return;
	stop();
	playback.pos = 0;
	
	SDL_PauseAudioDevice(dev, 0);
}
void Sound::stop()
{
	if(good and dev != 0)
	{
		SDL_PauseAudioDevice(dev, 1);
	}
}