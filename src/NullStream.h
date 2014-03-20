#ifndef NullStream_H
#define NullStream_H
#include <ostream>
struct NullStream{
	template <typename T>
	NullStream & operator << (T const & item)
	{
		return *this;
	}
	NullStream & operator << (std::ostream & (*func)(std::ostream&))
	{
		return *this;
	}
};
#endif
