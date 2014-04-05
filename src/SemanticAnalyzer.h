#ifndef SemanticAnalyzer_H
#define SemanticAnalyzer_H
#include <memory>
#include <map>
#include <string>
#include "Token.h"
#include "SyntaxAnalyzer.h"
class SemanticAnalyzer{
	std::map<std::string, Token> & data;
	void visitError(std::unique_ptr<AST> & tree);
	void visitAssignment(std::unique_ptr<AST> & tree);
	void visitTop(std::unique_ptr<AST> & tree);
public:
	void visit(std::unique_ptr<AST> & tree);
	SemanticAnalyzer(std::map<std::string, Token> & dataMap);
};
#endif
