#ifndef Evaluator_h
#define Evaluator_h
#include <map>
#include <string>
#include "Object.h"
#include "AST.h"
#include "ASTVisitor.h"
class Evaluator : public ASTVisitor{
	std::map<std::string, Object*> & data;
	std::map<AST*, Object*> objectTable;
	
	typedef ASTVisitor::visit_t visit_t;
	
	visit_t visitTop, visitAssignment,
		visitIdentifier, visitInteger, visitBoolean, visitReal, visitString,
		visitList,
		visitError;
public:
	Evaluator(std::map<std::string, Object*> & table) : data(table){}
};
#endif
