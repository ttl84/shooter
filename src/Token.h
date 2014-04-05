#ifndef Token_h
#define Token_h
#include "Char.h"
#include <string>
#include <ostream>
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
std::ostream& operator<<(std::ostream& os, const Token& obj);
#endif
