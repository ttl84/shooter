#ifndef SemanticAnalyzer_H
#define SemanticAnalyzer_H
#include <set>
#include <string>

#include "ASTVisitor.h"
class SemanticAnalyzer final: public ASTVisitor{
	std::set<std::string> symbolTable;
	
	void visitIdentifier	(std::unique_ptr<AST> &) override;
	void visitError		(std::unique_ptr<AST> &) override;
	std::string getName() const override
	{
		return "semantic analyzer";
	}
public:
	bool bad(void);
};
#endif
