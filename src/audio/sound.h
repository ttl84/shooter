#ifndef SOUND_H
#define SOUND_H
#include "SDL2/SDL_mixer.h"
#include <memory>
struct Sound {
	std::unique_ptr<Mix_Chunk, Mix_FreeChunk> data;

	Sound(char const * path)
		:data(Mix_LoadWAV(path))
	{
	}
};

#endif // SOUND_H

