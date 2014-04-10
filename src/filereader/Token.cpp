#include "Token.h"
std::ostream& operator<<(std::ostream& os, Token::Type type)
{
	#define MAKE_CASE(t) case Token::Type:: t : return os << #t
	switch(type){
		MAKE_CASE(NONE);
		MAKE_CASE(ERROR);
		MAKE_CASE(INTEGER);
		MAKE_CASE(REAL);
		MAKE_CASE(BOOLEAN);
		MAKE_CASE(IDENTIFIER);
		MAKE_CASE(STRING);
		MAKE_CASE(OPEN_SQUARE_BRACKET);
		MAKE_CASE(CLOSE_SQUARE_BRACKET);
		MAKE_CASE(ASSIGNMENT);
		MAKE_CASE(COMMA);
		MAKE_CASE(END);
	}
	#undef MAKE_CASE
	return os;
}
std::ostream& operator<<(std::ostream& os, const Token& obj)
{
	os << "TOKEN<" << obj.type << ", '" << obj.lexeme << "'>";
	return os;
}