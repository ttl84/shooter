#ifndef Evaluator_h
#define Evaluator_h
#include <map>
#include <string>
#include "Object.h"
#include "SyntaxAnalyzer.h"
class Evaluator{
	std::map<std::string, Object*> & data;
	void visitAssignment(std::unique_ptr<AST> & tree);
	void visitTop(std::unique_ptr<AST> & tree);
	
	Object * eval(std::unique_ptr<AST> & tree);
public:
	Evaluator(std::map<std::string, Object*> & table) : data(table){}
	void visit(std::unique_ptr<AST> & tree);
};
#endif
