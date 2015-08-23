#ifndef debug_h
#define debug_h
#include <iostream>
namespace debug{
	extern decltype(std::cout) & out;
	extern decltype(std::cout) & err;
}
#endif
