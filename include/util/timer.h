#ifndef TIMER_H
#define TIMER_H
#include <chrono>
namespace util {
class Timer{
	// This keeps track of the last time lap() or constructor was called
	std::chrono::high_resolution_clock::time_point sample;
public:
	Timer()
		:sample(std::chrono::high_resolution_clock::now())
	{
	}

	// Get the delta time in seconds, and update the current sample
	double lap()
	{
		auto past = sample;
		sample = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> difference = sample - past;
		return difference.count();
	}

	// Get the delta time since the last sample update
	double diff()
	{
		auto now = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> difference = now - sample;
		return difference.count();
	}
};
}
#endif // TIMER_H

