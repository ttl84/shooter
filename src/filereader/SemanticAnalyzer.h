#ifndef SemanticAnalyzer_H
#define SemanticAnalyzer_H
#include <memory>
#include <set>
#include <string>
#include "Token.h"
#include "SyntaxAnalyzer.h"
class SemanticAnalyzer : public ASTVisitor{
	std::set<std::string> symbolTable;
	bool badFlag;
	void semanticError(void);
public:
	void visit(std::unique_ptr<AST> & tree, std::unique_ptr<AST> & parent);
	bool bad(void);
};
#endif
