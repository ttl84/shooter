#ifndef AUDIO_H
#define AUDIO_H
#include <memory>
#include <string>
struct Sound;

class Audio{
	struct Data;
	std::unique_ptr<Data> data;
public:
	Audio();
	~Audio();

	Sound * load(std::string path);
	void play(Sound *);
};
#endif // AUDIO_H

