#ifndef Evaluator_h
#define Evaluator_h
#include <memory>
#include <map>
#include <vector>
#include <string>
#include "Object.h"
#include "AST.h"
#include "ASTVisitor.h"
class Evaluator final: public ASTVisitor{
private:
	std::map<std::string, Object*> & data;
	std::map<AST*, Object *> objectTable;
	std::vector<std::unique_ptr<Object>> & pool;
	
	#define DECL_VISIT(name)\
	void name (std::unique_ptr<AST> &) final override
	
	DECL_VISIT(visitAssignment);
	DECL_VISIT(visitIdentifier);
	DECL_VISIT(visitInteger);
	DECL_VISIT(visitBoolean);
	DECL_VISIT(visitReal);
	DECL_VISIT(visitString);
	DECL_VISIT(visitList);
	DECL_VISIT(visitError);
	#undef DECL_VISIT
	
	std::string getName() const override
	{
		return "evaluator";
	}
public:
	Evaluator(
		std::map<std::string, Object*> & d, 
		std::vector<std::unique_ptr<Object>> & p)
		: data(d), pool(p)
	{
	}
};
#endif
