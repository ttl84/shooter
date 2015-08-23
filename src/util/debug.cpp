#include "debug.h"
namespace debug {
	decltype(std::cout) & out = std::cout;
	decltype(std::cout) & err = std::cerr;
}
