#include "SyntaxAnalyzer.h"
AST * parseError(TokenStream & ts)
{
	return new AST{AST::Type::ERROR, ts.get()};
}
AST * parseIdentifier(TokenStream & ts)
{
	if(ts.peek().type != Token::Type::IDENTIFIER)
		return parseError(ts);
	return new AST{AST::Type::IDENTIFIER, ts.get()};
}
AST * parseString(TokenStream & ts)
{
	if(ts.peek().type != Token::Type::STRING)
		return parseError(ts);
	return new AST{AST::Type::STRING, ts.get()};
}
AST * parseInteger(TokenStream & ts)
{
	if(ts.peek().type != Token::Type::INTEGER)
		return parseError(ts);
	return new AST{AST::Type::INTEGER, ts.get()};
}
AST * parseReal(TokenStream & ts)
{
	if(ts.peek().type != Token::Type::REAL)
		return parseError(ts);
	return new AST{AST::Type::REAL, ts.get()};
}
AST * parseBoolean(TokenStream & ts)
{
	if(ts.peek().type != Token::Type::BOOLEAN)
		return parseError(ts);
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
		return parseError(ts);
	}
}
AST * parseAssignment(TokenStream & ts)
{
	if(ts.peek().type != Token::Type::IDENTIFIER)
		return parseError(ts);
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
		return parseError(ts);
	}
}
AST * parseTop(TokenStream & ts)
{
	AST * tree = new AST{AST::Type::TOP, ts.peek()};
	while(ts)
		tree->children.emplace_back(parseStatement(ts));
	return tree;
}
AST * parse(TokenStream & ts)
{
	return parseTop(ts);
}