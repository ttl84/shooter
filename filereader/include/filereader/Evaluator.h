#ifndef Evaluator_h
#define Evaluator_h
#include <memory>
#include <map>
#include <vector>
#include <string>
#include "Object.h"
#include "AST.h"
#include "ASTVisitor.h"
class Evaluator : public ASTVisitor{
	std::map<std::string, Object*> & data;
	std::map<AST*, Object *> objectTable;
	std::vector<std::unique_ptr<Object>> & pool;
	
	typedef ASTVisitor::visit_t visit_t;
	
	visit_t visitAssignment,
		visitIdentifier, visitInteger, visitBoolean, visitReal, visitString,
		visitList,
		visitError;
public:
	Evaluator(
		std::map<std::string, Object*> & d, 
		std::vector<std::unique_ptr<Object>> & p)
		: data(d), pool(p)
	{
	}
};
#endif
