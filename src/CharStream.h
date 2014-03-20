#ifndef CharStream_H
#define CharStream_H
#include <istream>
struct Char{
	char c;
	bool end;
	unsigned row;
	unsigned col;
	operator char(void) const
	{
		return c;
	}
};
class CharStream{
private:
	std::istream & is;
	unsigned row;
	unsigned col;
public:
	CharStream(std::istream & byteStream)
	:is(byteStream), row(0), col(0)
	{
		
	}
	
	Char get(void)
	{
		char next = is.get();
		Char ch{next, is.eof(), row, col};
		if(is)
		{
			if(next == '\n')
			{
				row++;
				col = 0;
			}
			else
			{
				col++;
			}
		}
		return ch;
	}
	Char peek(void)
	{
		char next = is.peek();
		return Char{next, is.eof(), row, col};
	}
	bool bad(void) const
	{
		return is.bad();
	}
	bool eof(void) const
	{
		return is.eof();
	}
	CharStream & operator >> (Char & ch)
	{
		ch = get();
		return *this;
	}
	operator bool(void) const
	{
		return is;
	}
};
#endif
