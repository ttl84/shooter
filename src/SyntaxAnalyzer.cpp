#include "SyntaxAnalyzer.h"
#include <iostream>
std::ostream & logError(Token & tok)
{
	return std::cerr << "syntax error (" << tok.row << ", " << tok.col << "): ";
}
AST * parseError(Token::Type expected, TokenStream & ts)
{
	Token tok = ts.get();
	logError(tok)
		<< "expected token type " << static_cast<int>(expected) << ", got "
		<< tok << std::endl;
	return new AST{AST::Type::ERROR, tok};
}
AST * unknownError(TokenStream & ts)
{
	Token tok = ts.get();
	logError(tok)
		<< "unknown token type"
		<< tok << std::endl;
	return new AST{AST::Type::ERROR, tok};
}
AST * parseIdentifier(TokenStream & ts)
{
	if(ts.peek().type != Token::Type::IDENTIFIER)
		return parseError(Token::Type::IDENTIFIER, ts);
	return new AST{AST::Type::IDENTIFIER, ts.get()};
}
AST * parseString(TokenStream & ts)
{
	if(ts.peek().type != Token::Type::STRING)
		return parseError(Token::Type::STRING, ts);
	return new AST{AST::Type::STRING, ts.get()};
}
AST * parseInteger(TokenStream & ts)
{
	if(ts.peek().type != Token::Type::INTEGER)
		return parseError(Token::Type::INTEGER, ts);
	return new AST{AST::Type::INTEGER, ts.get()};
}
AST * parseReal(TokenStream & ts)
{
	if(ts.peek().type != Token::Type::REAL)
		return parseError(Token::Type::REAL, ts);
	return new AST{AST::Type::REAL, ts.get()};
}
AST * parseBoolean(TokenStream & ts)
{
	if(ts.peek().type != Token::Type::BOOLEAN)
		return parseError(Token::Type::BOOLEAN, ts);
	return new AST{AST::Type::BOOLEAN, ts.get()};
}
AST * parseLiteral(TokenStream & ts)
{
	switch(ts.peek().type){
	case Token::Type::IDENTIFIER:
		return parseIdentifier(ts);
	case Token::Type::STRING:
		return parseString(ts);
	case Token::Type::INTEGER:
		return parseString(ts);
	case Token::Type::REAL:
		return parseReal(ts);
	case Token::Type::BOOLEAN:
		return parseBoolean(ts);
	default:
		return unknownError(ts);
	}
}
AST * parseAssignment(TokenStream & ts)
{
	if(ts.peek().type != Token::Type::IDENTIFIER)
		return parseError(Token::Type::IDENTIFIER, ts);
	AST * tree = new AST{AST::Type::ASSIGNMENT, ts.peek()};
	tree->children.emplace_back(parseIdentifier(ts));
	tree->children.emplace_back(parseLiteral(ts));
	return tree;
}
AST * parseStatement(TokenStream & ts)
{
	switch(ts.peek().type){
	case Token::Type::IDENTIFIER:
		return parseAssignment(ts);
	default:
		std::cerr << "parse statement: ";
		return unknownError(ts);
	}
}
AST * parseTop(TokenStream & ts)
{
	AST * tree = new AST{AST::Type::TOP, ts.peek()};
	while(ts.peek().type != Token::Type::END)
		tree->children.emplace_back(parseStatement(ts));
	return tree;
}
AST * parse(TokenStream & ts)
{
	return parseTop(ts);
}