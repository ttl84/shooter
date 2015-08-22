#ifndef Char_h
#define Char_h
struct Char{
	char c;
	bool end;
	unsigned row;
	unsigned col;
	operator char() const
	{
		return c;
	}
};
#endif
