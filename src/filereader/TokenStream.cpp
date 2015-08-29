#include "TokenStream.h"
#include <sstream>
#include <iostream>

namespace fr{
bool isEnd(Char const & ch)
{
	return ch.end;
}
bool isChar(Char const & ch, char c)
{
	return (not isEnd(ch)) and ch == c;
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
template<class T>
bool stringTo(std::string const & lexeme, T & output)
{
	T tmp;
	if(std::istringstream(lexeme) >> tmp)
	{
		output = tmp;
		return true;
	}
	else
		return false;
}
bool toLong(std::string const & lexeme, long & output)
{
	return stringTo(lexeme, output);
	
}
bool toReal(std::string const & lexeme, double & output)
{
	return stringTo(lexeme, output);
}

class NumMatcher{
private:
	CharStream & cs;
public:
	std::string lexeme;
	bool err;
	NumMatcher(CharStream & stream):cs(stream), err(false)
	{
		number();
	}
private:
	//number -> sign integer fraction exponent
	void number()
	{
		sign();
		integer();
		fraction();
		exponent();
	}
	//integer -> digit digit*
	void integer()
	{
		do digit(); while(not err and isDigit(cs.peek()));
	}
	//fraction -> epsilon | . integer
	void fraction()
	{
		if(err)
			return;
		Char ch = cs.peek();
		if(isChar(ch, '.'))
		{
			lexeme += cs.get();
			integer();
		}
	}
	//exponent -> epsilon | e (+ | - | epsilon) integer
	void exponent()
	{
		if(err)
			return;
		Char ch = cs.peek();
		if(isChar(ch, 'e') or isChar(ch, 'E'))
		{
			lexeme += cs.get();
			sign();
			integer();
		}
	}
	//terminals
	//sign -> (- | epsilon)
	void sign()
	{
		if(err)
			return;
		Char ch = cs.peek();
		if(isChar(ch, '-'))
			lexeme += cs.get();
	}
	void digit()
	{
		if(err)
			return;
		auto ch = cs.get();
		if(isDigit(ch))
			lexeme += ch;
		else
			err = true;
	}
};
Token readNumber(CharStream & cs)
{
	Char first = cs.peek();
	Token token(first);
	
	NumMatcher matcher(cs);
	token.lexeme = matcher.lexeme;
	token.type = Token::Type::ERROR;
	if(not matcher.err)
	{
		if(toReal(token.lexeme, token.datum.real))
			token.type = Token::Type::REAL;
		if(toLong(token.lexeme, token.datum.integer))
			token.type = Token::Type::INTEGER;
	}
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
		tok.type = Token::Type::BEGIN_LIST;
	
	else if(first == ']')
		tok.type = Token::Type::END_LIST;
	
	else if(first == ':' and cs.peek() == '=')
	{
		tok.lexeme += cs.get();
		tok.type = Token::Type::ASSIGNMENT;
	}
	
	else if(first == ',')
		tok.type = Token::Type::CONTINUE_LIST;
	
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

Token TokenStream::get(void)
{
	Token token = nextToken;
	nextToken = readToken(cs);
	eofFlag = (token.type == Token::Type::END);
	return token;
}
Token const & TokenStream::peek(void) const
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

TokenStream::operator bool(void) const
{
	return (not eof()) and (not bad());
}
} // end namespace fr
