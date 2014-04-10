#include "SyntaxAnalyzer.h"
#include <iostream>
std::ostream & logError(Token & tok)
{
	return std::cerr << "syntax error (" << tok.row << ", " << tok.col << "): ";
}
template<typename T>
AST * parseError(T expected, TokenStream & ts)
{
	Token tok = ts.get();
	logError(tok)
		<< "expected token type " << expected << ", got "
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
void expect(Token::Type type, TokenStream & ts)
{
	Token tok = ts.get();
	Token expected(type);
	if(tok.type != type)
	{
		logError(tok)
			<< "expected " << expected << ", got "
			<< tok << std::endl;
	}
		
}
bool beginsIdentifier(TokenStream & ts)
{
	return ts.peek().type == Token::Type::IDENTIFIER;
}
bool beginsString(TokenStream & ts)
{
	return ts.peek().type == Token::Type::STRING;
}
bool beginsInteger(TokenStream & ts)
{
	return ts.peek().type == Token::Type::INTEGER;
}
bool beginsReal(TokenStream & ts)
{
	return ts.peek().type == Token::Type::REAL;
}
bool beginsBoolean(TokenStream & ts)
{
	return ts.peek().type == Token::Type::BOOLEAN;
}
bool beginsLiteral(TokenStream & ts)
{
	return beginsIdentifier(ts) or
		beginsString(ts) or
		beginsInteger(ts) or
		beginsReal(ts) or
		beginsBoolean(ts);
}
bool beginsList(TokenStream & ts)
{
	return ts.peek().type == Token::Type::OPEN_SQUARE_BRACKET;
}
bool beginsExpression(TokenStream & ts)
{
	return beginsList(ts) or beginsLiteral(ts);
}
AST * parseExpression(TokenStream & ts);
AST * parseIdentifier(TokenStream & ts)
{
	if(not beginsIdentifier(ts))
		return parseError(Token::Type::IDENTIFIER, ts);
	else
		return new AST{AST::Type::IDENTIFIER, ts.get()};
}

AST * parseString(TokenStream & ts)
{
	if(not beginsString(ts))
		return parseError(Token::Type::STRING, ts);
	else
		return new AST{AST::Type::STRING, ts.get()};
}

AST * parseInteger(TokenStream & ts)
{
	if(not beginsInteger(ts))
		return parseError(Token::Type::INTEGER, ts);
	else
		return new AST{AST::Type::INTEGER, ts.get()};
}

AST * parseReal(TokenStream & ts)
{
	if(not beginsReal(ts))
		return parseError(Token::Type::REAL, ts);
	else
		return new AST{AST::Type::REAL, ts.get()};
}

AST * parseBoolean(TokenStream & ts)
{
	if(not beginsBoolean(ts))
		return parseError(Token::Type::BOOLEAN, ts);
	else
		return new AST{AST::Type::BOOLEAN, ts.get()};
}

AST * parseLiteral(TokenStream & ts)
{
	if(not beginsLiteral(ts))
		return parseError("literal", ts);
	else if(beginsIdentifier(ts))
		return parseIdentifier(ts);
	else if(beginsString(ts))
		return parseString(ts);
	else if(beginsInteger(ts))
		return parseInteger(ts);
	else if(beginsReal(ts))
		return parseReal(ts);
	else if(beginsBoolean(ts))
		return parseBoolean(ts);
	else
		return unknownError(ts);
}
AST * parseList(TokenStream & ts)
{
	if(not beginsList(ts))
		return parseError("list", ts);
	AST * tree = new AST{AST::Type::LIST, ts.peek()};
	expect(Token::Type::OPEN_SQUARE_BRACKET, ts);
	while(beginsExpression(ts))
	{
		tree->children.emplace_back(parseExpression(ts));
		if(ts.peek().type == Token::Type::COMMA)
			ts.get();
	}
	if(ts.peek().type == Token::Type::CLOSE_SQUARE_BRACKET)
	{
		ts.get();
		return tree;
	}
	else
	{
		tree->type = AST::Type::ERROR;
		return tree;
	}
}
AST * parseExpression(TokenStream & ts)
{
	if(beginsList(ts))
		return parseList(ts);
	else if(beginsLiteral(ts))
		return parseLiteral(ts);
	else
		return unknownError(ts);
		
}
AST * parseAssignment(TokenStream & ts)
{
	if(ts.peek().type != Token::Type::IDENTIFIER)
		return parseError(Token::Type::IDENTIFIER, ts);
	AST * tree = new AST{AST::Type::ASSIGNMENT, ts.peek()};
	tree->children.emplace_back(parseIdentifier(ts));
	expect(Token::Type::ASSIGNMENT, ts);
	tree->children.emplace_back(parseExpression(ts));
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
