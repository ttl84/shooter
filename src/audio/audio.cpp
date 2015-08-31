#include "audio.h"
#include "sound.h"
#include "SDL2/SDL.h"
struct Audio::Data{
	std::unordered_map<std::string, Sound > loaded;
};
Audio::Audio()
	:data(new Data)
{
	SDL_InitSubSystem(SDL_INIT_AUDIO);

	int samplingFreq = 22050;
	unsigned soundFormat = AUDIO_S8;
	int channels = 2;
	int chunkSize = 2048;
	Mix_OpenAudio(samplingFreq, soundFormat, channels, chunkSize);

}

Audio::~Audio()
{
	// Before anything, free all chunks
	data->loaded.clear();

	Mix_CloseAudio();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

Sound* Audio::load(std::string path)
{
	auto it = data->loaded.find(path);
	if(it == data->loaded.end()) {
		Sound sound(path.c_str());
		if(sound.data != nullptr) {
			auto res = data->loaded.emplace(std::move(path), std::move(sound));
			it = res.first;
		} else {
			return nullptr;
		}
	}
	return &it->second;
}

void Audio::play(Sound* sound)
{
	Mix_PlayChannel(-1, sound, 0);
}
