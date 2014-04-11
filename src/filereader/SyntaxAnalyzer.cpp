#include "SyntaxAnalyzer.h"
#include <iostream>
namespace{
std::ostream & logError(Token & tok)
{
	return std::cerr << "syntax error (" << tok.row << ", " << tok.col << "): ";
}
template<typename T>
AST * parseError(T expected, TokenStream & ts)
{
	Token tok = ts.get();
	logError(tok)
		<< "expected " << expected << ", got "
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
bool beginsAssignment(TokenStream & ts)
{
	return beginsIdentifier(ts);
}
AST * parseExpression(TokenStream & ts);
AST * parseIdentifier(TokenStream & ts)
{
	if(beginsIdentifier(ts))
		return new AST(AST::Type::IDENTIFIER, ts.get());
	else
		return parseError("identifier literal", ts);
}

AST * parseString(TokenStream & ts)
{
	if(beginsString(ts))
		return new AST(AST::Type::STRING, ts.get());
	else
		return parseError("string", ts);
}

AST * parseInteger(TokenStream & ts)
{
	if(beginsInteger(ts))
		return new AST(AST::Type::INTEGER, ts.get());
	else
		return parseError("integer literal", ts);
}

AST * parseReal(TokenStream & ts)
{
	if(beginsReal(ts))
		return new AST(AST::Type::REAL, ts.get());
	else
		return parseError("real number literal", ts);
		
}

AST * parseBoolean(TokenStream & ts)
{
	if(beginsBoolean(ts))
		return new AST(AST::Type::BOOLEAN, ts.get());
	else
		return parseError("boolean literal", ts);
}

AST * parseLiteral(TokenStream & ts)
{
	if(beginsLiteral(ts))
	{
		if(beginsIdentifier(ts))
			return parseIdentifier(ts);
		
		else if(beginsString(ts))
			return parseString(ts);
		
		else if(beginsInteger(ts))
			return parseInteger(ts);
		
		else if(beginsReal(ts))
			return parseReal(ts);
		
		else if(beginsBoolean(ts))
			return parseBoolean(ts);
	}
	return parseError("literal expression", ts);
}
AST * parseList(TokenStream & ts)
{
	if(not beginsList(ts))
		return parseError("list", ts);
	
	AST * tree = new AST(AST::Type::LIST, ts.peek());
	expect(Token::Type::OPEN_SQUARE_BRACKET, ts);
	while(beginsExpression(ts))
	{
		tree->append(parseExpression(ts));
		if(ts.peek().type == Token::Type::COMMA)
			ts.get();
	}
	expect(Token::Type::CLOSE_SQUARE_BRACKET, ts);
	return tree;
}

AST * parseExpression(TokenStream & ts)
{
	if(beginsExpression(ts))
	{
		if(beginsList(ts))
			return parseList(ts);
		else if(beginsLiteral(ts))
			return parseLiteral(ts);
	}
	return parseError("expression", ts);
		
}
AST * parseAssignment(TokenStream & ts)
{
	if(not beginsAssignment(ts))
		return parseError("assignment statement", ts);
	
	AST * tree = new AST(AST::Type::ASSIGNMENT, ts.peek());
	tree->append(parseIdentifier(ts));
	expect(Token::Type::ASSIGNMENT, ts);
	tree->append(parseExpression(ts));
	return tree;
}
AST * parseStatement(TokenStream & ts)
{
	if(beginsAssignment(ts))
		return parseAssignment(ts);
	else
		return parseError("statement", ts);
}
AST * parseTop(TokenStream & ts)
{
	AST * tree = new AST(AST::Type::TOP, ts.peek());
	while(ts.peek().type != Token::Type::END)
		tree->append(parseStatement(ts));
	return tree;
}
}// end namespace
std::unique_ptr<AST> parse(TokenStream & ts)
{
	return std::unique_ptr<AST>(parseTop(ts));
}
