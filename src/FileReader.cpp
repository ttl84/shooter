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
bool FileReader::getString(std::string key, std::string & output) const
{
	Token tok = data.at(key);
	if(tok.type != Token::Type::STRING)
		return false;
	else
	{
		output = tok.lexeme;
		return true;
	}
}
bool FileReader::getInteger(std::string key, long & output) const
{
	Token tok = data.at(key);
	if(tok.type != Token::Type::INTEGER)
		return false;
	else
	{
		output = tok.datum.integer;
		return true;
	}
}
bool FileReader::getReal(std::string key, double & output) const
{
	Token tok = data.at(key);
	if(tok.type != Token::Type::REAL)
		return false;
	else
	{
		output = tok.datum.real;
		return true;
	}
}
bool FileReader::getBoolean(std::string key, bool & output) const
{
	Token tok = data.at(key);
	if(tok.type != Token::Type::BOOLEAN)
		return false;
	else
	{
		output = tok.datum.boolean;
		return true;
	}
}