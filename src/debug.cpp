#include "debug.h"
#ifdef DEBUG
namespace debug{
	decltype(std::cout) & out = std::cout;
	decltype(std::cerr) & err = std::cerr;
}
#else
#include "NullStream.h"
namespace debug{
	NullStream out;
	NullStream err;
	
}
#endif