#ifndef debug_h
#define debug_h
#include <iostream>
#ifdef DEBUG
namespace debug{
	extern decltype(std::cout) & log;
}
#else
#include "NullStream.h"
namespace debug{
	extern NullStream log;
}
#endif
#endif
