#ifndef Evaluator_h
#define Evaluator_h
#include <memory>
#include <map>
#include <string>
#include "Object.h"
#include "AST.h"
#include "ASTVisitor.h"
class Evaluator : public ASTVisitor{
	std::map<std::string, std::shared_ptr<Object>> & data;
	std::map<AST*, std::shared_ptr<Object>> objectTable;
	
	typedef ASTVisitor::visit_t visit_t;
	
	visit_t visitAssignment,
		visitIdentifier, visitInteger, visitBoolean, visitReal, visitString,
		visitList,
		visitError;
public:
	Evaluator(std::map<std::string, std::shared_ptr<Object>> & table) : data(table){}
};
#endif
