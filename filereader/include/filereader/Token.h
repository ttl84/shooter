#ifndef Token_h
#define Token_h
#include "Char.h"
#include <string>
#include <ostream>
struct Token final{
	enum class Type : unsigned{
		INTEGER, REAL, BOOLEAN, IDENTIFIER, STRING,
		OPEN_SQUARE_BRACKET, CLOSE_SQUARE_BRACKET,
		ASSIGNMENT,
		COMMA,
		ERROR, COMMENT, END
	} type;
	union{
		long integer;
		double real;
		bool boolean;
	} datum;
	std::string lexeme;
	unsigned row, col;
	
	Token(Type t, Char first):type(t), row(first.row), col(first.col){}
	Token(Type t):type(t), row(0), col(0){}
	Token(Char first):row(first.row), col(first.col){}
	Token():Token(Type::ERROR){}
};
std::ostream& operator<<(std::ostream& os, const Token& obj);
#endif
