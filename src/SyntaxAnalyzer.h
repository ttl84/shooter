#ifndef SyntaxAnalyzer_H
#define SyntaxAnalyzer_H
#include <vector>
struct AST{
	enum class Type{
		ASSIGNMENT, IDENTIFIER, REAL, INTEGER, BOOLEAN, STRING, ERROR, NONE
	} type;
	std::vector<std::unique_ptr<AST>> children;
};
#endif