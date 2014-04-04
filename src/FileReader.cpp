#include "FileReader.h"
#include <iostream>
#include <memory>
#include "SyntaxAnalyzer.h"
#include "SemanticAnalyzer.h"
FileReader::FileReader(std::istream & is)
{
	CharStream cs(is);
	TokenStream ts(cs);
	Token tok;
	//while(ts >> tok)
	//	std::cout << tok.row << ' ' << tok.col << ' ' << static_cast<int>(tok.type) << ' ' << tok.lexeme << std::endl;
	std::unique_ptr<AST> tree(parse(ts));
	SemanticAnalyzer(data).visit(tree);
	
}
std::string FileReader::getString(std::string key) const
{
	Token tok = data.at(key);
	if(tok.type != Token::Type::STRING)
		throw "value of key is not a string";
	return tok.lexeme;
}
long FileReader::getInteger(std::string key) const
{
	Token tok = data.at(key);
	if(tok.type != Token::Type::INTEGER)
		throw "value of key is not an integer";
	return tok.datum.integer;
}
double FileReader::getReal(std::string key) const
{
	Token tok = data.at(key);
	if(tok.type != Token::Type::REAL)
		throw "value of key is not real";
	return tok.datum.real;
}
bool FileReader::getBoolean(std::string key) const
{
	Token tok = data.at(key);
	if(tok.type != Token::Type::BOOLEAN)
		throw "value of key is not boolean";
	return tok.datum.boolean;
}