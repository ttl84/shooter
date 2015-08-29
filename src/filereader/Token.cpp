#include "Token.h"
namespace fr{
std::ostream& operator<<(std::ostream& os, Token::Type type)
{
	#define MAKE_CASE(t) case Token::Type:: t : return os << #t
	switch(type){
		MAKE_CASE(ERROR);
		MAKE_CASE(INTEGER);
		MAKE_CASE(REAL);
		MAKE_CASE(BOOLEAN);
		MAKE_CASE(IDENTIFIER);
		MAKE_CASE(STRING);
		MAKE_CASE(BEGIN_LIST);
		MAKE_CASE(END_LIST);
		MAKE_CASE(ASSIGNMENT);
		MAKE_CASE(CONTINUE_LIST);
		MAKE_CASE(COMMENT);
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
} // end namespace fr
