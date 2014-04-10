#ifndef SemanticAnalyzer_H
#define SemanticAnalyzer_H
#include <set>
#include <string>

#include "ASTVisitor.h"
class SemanticAnalyzer : public ASTVisitor{
	std::set<std::string> symbolTable;
	bool badFlag;
	void semanticError(void);
	
	typedef ASTVisitor::visit_t visit_t;
	
	visit_t visitTop, visitAssignment,
		visitIdentifier, visitInteger, visitBoolean, visitReal, visitString,
		visitList, visitError;
	
public:
	bool bad(void);
};
#endif
