#include "util/timer.h"
#include <vector>
#include <iostream>

int main()
{
	util::Timer timer;
	std::vector<double> dts;
	for(unsigned i = 0; i < 100;) {
		double diff = timer.diff();
		if(diff != 0) {
			timer.lap();
			dts.push_back(diff);
			i++;
		}
	}
	for(double dt : dts) {
		std::cout << dt << '\n';
	}
	return 0;
}
