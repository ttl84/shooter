#include "Token.h"
std::ostream& operator<<(std::ostream& os, const Token& obj)
{
	os << "TOKEN<" << static_cast<int>(obj.type) << ", " << obj.lexeme << ">";
	return os;
}