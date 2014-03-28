#ifndef SyntaxAnalyzer_H
#define SyntaxAnalyzer_H
#include <vector>
#include <memory>
#include "TokenStream.h"
struct AST{
	enum class Type{
		TOP, ASSIGNMENT, IDENTIFIER, REAL, INTEGER, BOOLEAN, STRING, ERROR, NONE
	} type;
	Token start;
	std::vector<std::unique_ptr<AST>> children;
};
AST * parse(TokenStream & ts);
#endif
