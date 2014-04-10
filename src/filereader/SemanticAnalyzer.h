#ifndef SemanticAnalyzer_H
#define SemanticAnalyzer_H
#include <memory>
#include <set>
#include <string>
#include "Token.h"
#include "SyntaxAnalyzer.h"
#include "ASTVisitor.h"
class SemanticAnalyzer : public ASTVisitor{
	std::set<std::string> symbolTable;
	bool badFlag;
	void semanticError(void);
	
public:
	bool bad(void);
};
#endif
