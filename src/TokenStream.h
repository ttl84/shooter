#ifndef TokenStream_H
#define TokenStream_H
#include "CharStream.h"
struct Token{
	enum class Type{
		NONE, INTEGER, REAL, BOOLEAN, IDENTIFIER, STRING, ERROR, END
	} type;
	union{
		long integer;
		double real;
		bool boolean;
	} datum;
	std::string lexeme;
	unsigned row, col;
	Token(Type t):type(t){}
	Token(Type t, Char first):type(t), row(first.row), col(first.col){}
	Token(Char first):row(first.row), col(first.col){}
	Token(void):type(Type::NONE){}
};

class TokenStream{
private:
	CharStream & cs;
	Token nextToken;
	bool eofFlag;
public:
	TokenStream(CharStream & charStream);
	
	Token get(void);
	Token peek(void) const;

	bool bad(void) const;
	bool eof(void) const;
	TokenStream & operator >>(Token & token);
	operator bool(void) const;
};
#endif
