#include "util/timer.h"
#include <chrono>
namespace util {
struct Timer::Data{
	std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::seconds> sample;
};

Timer::Timer()
	:data(new Timer::Data)
{
	if(data != nullptr) {
		data->sample = std::chrono::high_resolution_clock::now();
	} else {
		throw "failed to create timer";
	}
}
double Timer::lap()
{
	auto past = data->sample;
	data->sample = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> difference = data->sample - past;
	return difference.count();
}
}
