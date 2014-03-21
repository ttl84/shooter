#include "FileReader.h"
#include <fstream>
#include <iostream>
FileReader::FileReader(std::istream & is)
{
	CharStream cs(is);
	TokenStream ts(cs);
	
	Token tok;
	
	while(ts>>tok)
		std::cout << tok.lexeme << ' ' << tok.datum.real << ' ' <<
		tok.datum.integer <<  ' ' << (tok.type != Token::Type::ERROR) << '\n';
	
}