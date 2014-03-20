#include "debug.h"
#ifdef DEBUG
namespace debug{
	decltype(std::cout) & log = std::cout;
}
#else
#include "NullStream.h"
namespace debug{
	NullStream log;
}
#endif