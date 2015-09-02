#ifndef TIMER_H
#define TIMER_H
#include <memory>
namespace util {
class Timer{
	struct Data;
	std::unique_ptr<Data> data;
public:
	Timer();
	~Timer();
	double lap();
};
}
#endif // TIMER_H

