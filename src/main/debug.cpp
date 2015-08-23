#include "debug.h"
namespace debug{
	decltype(std::cout) & out = std::cout;
	decltype(std::cerr) & err = std::cerr;
}