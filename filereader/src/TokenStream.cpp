#include "TokenStream.h"
#include <sstream>
#include <iostream>

namespace{
bool isEnd(Char const & ch)
{
	return ch.end;
}
bool isLowerCase(Char const & ch)
{
	return (not isEnd(ch)) and 
		ch.c >= 'a' and ch.c <= 'z';
}
bool isUpperCase(Char const & ch)
{
	return (not isEnd(ch)) and 
		ch.c >= 'A' and ch.c <= 'Z';
}
bool isDigit(Char const & ch)
{
	return (not isEnd(ch)) and 
		ch.c >= '0' and ch.c <= '9';
}
bool isWhitespace(Char const & ch)
{
	return (not isEnd(ch)) and 
		(ch.c == ' ' or ch.c == '\n' or ch.c == '\t');
}
bool beginsIdentifier(Char const & ch)
{
	return (not isEnd(ch)) and
		(isLowerCase(ch) or isUpperCase(ch) or ch.c == '_');
}
bool beginsString(Char const & ch)
{
	return (not isEnd(ch)) and ch.c == '"';
}
bool beginsNumber(Char const & ch)
{
	return (not isEnd(ch)) and
		(ch.c == '-' or isDigit(ch));
}
bool beginsOperator(Char const & ch)
{
	return (not isEnd(ch)) and
		(ch == ':' or ch == '[' or ch == ']' or ch == ',');
}
bool beginsComment(Char ch)
{
	return (not isEnd(ch)) and ch == ';';
}
bool inNumber(Char const & ch)
{
	return isDigit(ch) or ch.c == '.' or ch.c == 'e';
}
bool inIdentifier(Char const & ch)
{
	return beginsIdentifier(ch) or isDigit(ch);
}
bool inString(Char const & ch)
{
	return not (isEnd(ch) or beginsString(ch));
}
bool toLong(std::string const & lexeme, long & output)
{
	return std::istringstream(lexeme) >> output;
	
}
bool toReal(std::string const & lexeme, double & output)
{
	return std::istringstream(lexeme) >> output;
}
Token readNumber(CharStream & cs)
{
	std::string lexeme;
	Char first = cs.peek();
	Char ch = first;
	Token token(first);
	for(unsigned state = 0, running = 1; running; )
	{
		switch(state){
		case 0: //negation sign
			if(ch.c == '-')
			{
				lexeme += ch.c;
				cs.get();
				ch = cs.peek();
			}
			state = 1;
			break;
		case 1: // first integer
			if(isDigit(ch))
			{
				lexeme += ch.c;
				cs.get();
				ch = cs.peek();
				state = 2;
			}
			else
				state = -1;
			break;
		case 2: //trailing integer
			if(isDigit(ch))
			{
				lexeme += ch.c;
				cs.get();
				ch = cs.peek();
			}
			else if(ch.c == '.')
			{
				lexeme += ch.c;
				cs.get();
				ch = cs.peek();
				state = 3;
			}
			else if(ch.c == 'e')
			{
				lexeme += ch.c;
				cs.get();
				ch = cs.peek();
				state = 5;
			}
			else
			{
				token.type = Token::Type::INTEGER;
				running = 0;
			}
			break;
		case 3: //first integer after decimal point
			if(isDigit(ch))
			{
				lexeme += ch.c;
				cs.get();
				ch = cs.peek();
				state = 4;
			}
			else
				state = -1;
			break;
		case 4: //trailing decimal or exponent
			if(isDigit(ch))
			{
				lexeme += ch.c;
				cs.get();
				ch = cs.peek();
			}
			else if(ch.c == 'e' or ch.c == 'E')
			{
				lexeme += ch.c;
				cs.get();
				ch = cs.peek();
				state = 5;
			}
			else
			{
				token.type = Token::Type::REAL;
				running = 0;
			}
			break;
		case 5: // after exponent, maybe sign
			if(ch.c == '-' or ch.c == '+')
			{
				lexeme += ch.c;
				cs.get();
				ch = cs.peek();
			}
			state = 6;
			break;
		case 6: // first integer of exponent
			if(isDigit(ch))
			{
				lexeme += ch.c;
				cs.get();
				ch = cs.peek();
				state = 7;
			}
			else
				state = -1;
			break;
		case 7: // trailing exponent
			if(isDigit(ch))
			{
				lexeme += ch.c;
				cs.get();
				ch = cs.peek();
			}
			else
			{
				token.type = Token::Type::REAL;
				running = 0;
			}
			break;
		default: //error
			if(not (isEnd(ch) or isWhitespace(ch)))
			{
				lexeme += ch;
				cs.get();
				ch = cs.peek();
			}
			else
			{
				token.type = Token::Type::ERROR;
				running = 0;
			}
		}
	}
	if(token.type == Token::Type::INTEGER)
	{
		if(not toLong(lexeme, token.datum.integer))
			token.type = Token::Type::ERROR;
	}
	else if(token.type == Token::Type::REAL)
	{
		if(not toReal(lexeme, token.datum.real))
			token.type = Token::Type::ERROR;
	}
	else
		token.type = Token::Type::ERROR;
	token.lexeme = lexeme;
	return token;
}
Token readIdentifier(CharStream & cs)
{
	Char first = cs.peek();
	Token tok(Token::Type::IDENTIFIER, first);
	while(inIdentifier(cs.peek()))
		tok.lexeme += cs.get();
	return tok;
}
Token readString(CharStream & cs)
{
	Char first = cs.get();// eat open quote
	Token tok(Token::Type::STRING, first);
	
	while(inString(cs.peek()))
		tok.lexeme += cs.get();
	
	Char last = cs.get(); //eat quote
	if(last.c != '"')
		tok.type = Token::Type::ERROR;
	
	return tok;
}
Token readOperator(CharStream & cs)
{
	Char first = cs.get();
	Token tok(first);
	
	if(first == '[')
		tok.type = Token::Type::OPEN_SQUARE_BRACKET;
	
	else if(first == ']')
		tok.type = Token::Type::CLOSE_SQUARE_BRACKET;
	
	else if(first == ':' and cs.peek() == '=')
	{
		tok.lexeme += cs.get();
		tok.type = Token::Type::ASSIGNMENT;
	}
	
	else if(first == ',')
		tok.type = Token::Type::COMMA;
	
	else
		tok.type = Token::Type::ERROR;
	return tok;
}
	
Token readError(CharStream & cs)
{
	Char first = cs.get();
	Token tok(Token::Type::ERROR, first);
	tok.lexeme += first;
	while(not (isWhitespace(cs.peek()) or isEnd(cs.peek())))
		tok.lexeme += cs.get();
	return tok;
}
Token  maybeKeyword(Token token)
{
	if(token.type == Token::Type::IDENTIFIER)
	{
		if(token.lexeme == "true")
		{
			token.type = Token::Type::BOOLEAN;
			token.datum.boolean = true;
		}
		else if(token.lexeme == "false")
		{
			token.type = Token::Type::BOOLEAN;
			token.datum.boolean = false;
		}
	}
	return token;
}
Token readComment(CharStream & cs)
{
	Char first = cs.get();
	Token tok(Token::Type::COMMENT, first);
	tok.lexeme += first;
	while(not (isEnd(cs.peek()) or cs.peek() == '\n'))
		tok.lexeme += cs.get();
	return tok;
}
void skipWhitespace(CharStream & cs)
{
	while(isWhitespace(cs.peek()))
		cs.get();
}
Token readToken(CharStream & cs)
{
	skipWhitespace(cs);
	
	
	Char next = cs.peek();
	if(isEnd(next))
		return Token(Token::Type::END);
	else if(beginsComment(next))
	{
		readComment(cs);
		return readToken(cs);
	}
	
	else if(beginsOperator(next))
		return readOperator(cs);
	
	else if(beginsIdentifier(next))
		return maybeKeyword(readIdentifier(cs));
	
	else if(beginsString(next))
		return readString(cs);
	
	else if(beginsNumber(next))
		return readNumber(cs);
	
	else
		return readError(cs);
}
}//end namespace
Token TokenStream::get(void)
{
	Token token = nextToken;
	nextToken = readToken(cs);
	eofFlag = (token.type == Token::Type::END);
	return token;
}
Token TokenStream::peek(void) const
{
	return nextToken;
}
bool TokenStream::bad(void) const
{
	return cs.bad();
}
bool TokenStream::eof(void) const
{
	return eofFlag;
}
TokenStream::TokenStream(CharStream & charStream)
:cs(charStream), eofFlag(false)
{
	get();
}
TokenStream & TokenStream::operator >>(Token & token)
{
	token = get();
	return *this;
}

TokenStream::operator bool(void) const
{
	return (not eof()) and (not bad());
}