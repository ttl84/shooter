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
	return not ch.end and 
		ch.c >= 'a' and ch.c <= 'z';
}
bool isUpperCase(Char const & ch)
{
	return not ch.end and 
		ch.c >= 'A' and ch.c <= 'Z';
}
bool isDigit(Char const & ch)
{
	return not ch.end and 
		ch.c >= '0' and ch.c <= '9';
}
bool isWhitespace(Char const & ch)
{
	return not ch.end and 
		(ch.c == ' ' or ch.c == '\n' or ch.c == '\t');
}
bool beginsIdentifier(Char const & ch)
{
	return not ch.end and
		(isLowerCase(ch) or isUpperCase(ch) or ch.c == '_');
}
bool beginsString(Char const & ch)
{
	return not ch.end and ch.c == '"';
}
bool beginsNumber(Char const & ch)
{
	return not ch.end and
		(ch.c == '-' or isDigit(ch));
}
bool inNumber(Char const & ch)
{
	return isDigit(ch) or ch.c == '.' or ch.c == 'e';
}
bool inIdentifier(Char const & ch)
{
	return beginsIdentifier(ch) or isDigit(ch);
}
bool toInt(std::string const & lexeme, int & output)
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
	Token token;
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
			else if(ch.c == 'e')
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
		case 5: // after exponent, maybe negation
			if(ch.c == '-')
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
			token.type = Token::Type::ERROR;
			running = 0;
		}
	}
	if(token.type == Token::Type::INTEGER)
	{
		if(not toInt(lexeme, token.datum.integer))
			token.type = Token::Type::ERROR;
	}
	else if(token.type == Token::Type::INTEGER)
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
	std::string lexeme;
	Char first = cs.get();
	lexeme += first.c;
	for(Char ch = cs.peek(); inIdentifier(ch); ch = cs.peek())
	{
		lexeme += ch.c;
		cs.get();
	}
	Token token{Token::Type::IDENTIFIER, first};
	token.lexeme = lexeme;
	return token;
}
Token readString(CharStream & cs)
{
	std::string lexeme;
	Char first = cs.get();// eat open quote
	for(Char ch = cs.peek(); not beginsString(ch); ch = cs.peek())
	{
		lexeme += ch.c;
		cs.get();
	}
	cs.get(); //eat quote
	Token token{Token::Type::STRING, first};
	token.lexeme = lexeme;
	return token;
}
Token readError(CharStream & cs)
{
	std::string lexeme;
	Char first = cs.get();
	lexeme += first.c;
	for(Char ch = cs.peek(); not isWhitespace(ch); ch = cs.peek())
	{
		lexeme += ch.c;
		cs.get();
	}
	Token token{Token::Type::ERROR, first};
	token.lexeme = lexeme;
	return token;
}
Token maybeKeyword(Token token)
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
	{
		Token token;
		token.type = Token::Type::END;
		return token;
	}
	else if(beginsString(next))
		return readString(cs);
	else if(beginsNumber(next))
		return readNumber(cs);
	else if(beginsIdentifier(next))
		return maybeKeyword(readIdentifier(cs));
	else
		return readError(cs);
}
}
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