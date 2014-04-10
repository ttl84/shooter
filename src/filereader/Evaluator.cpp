#include "Evaluator.h"
#include <iostream>
Object * Evaluator::eval(std::unique_ptr<AST> & tree)
{
	Object * obj = nullptr;
	switch(tree->type){
	case AST::Type::BOOLEAN:
		obj = new Object{Object::Type::BOOLEAN};
		obj->datum.boolean = tree->start.datum.boolean;
		return obj;
	case AST::Type::INTEGER:
		obj = new Object{Object::Type::INTEGER};
		obj->datum.integer = tree->start.datum.integer;
		return obj;
	case AST::Type::REAL:
		obj = new Object{Object::Type::REAL};
		obj->datum.real = tree->start.datum.real;
		return obj;
	case AST::Type::STRING:
		obj = new Object{Object::Type::STRING};
		obj->datum.string = new std::string(tree->start.lexeme);
		return obj;
	case AST::Type::LIST:
		obj = new Object{Object::Type::LIST};
		obj->datum.list = new std::vector<Object *>;
		for(auto & child : tree->children)
		{
			Object * childObj = eval(child);
			obj->datum.list->push_back(childObj);
		}
		return obj;
	case AST::Type::IDENTIFIER:
		return data.at(tree->start.lexeme);
	default:
		std::cerr << "evaluator error" << std::endl;
		return obj;
	}
}

void Evaluator::visitAssignment(std::unique_ptr<AST> & tree)
{
	auto & left = tree->children[0];
	auto & right = tree->children[1];
	visit(left);
	visit(right);
	data[left->start.lexeme] = eval(right);
}
void Evaluator::visitTop(std::unique_ptr<AST> & tree)
{
	for(auto & child : tree->children)
		visit(child);
}
void Evaluator::visit(std::unique_ptr<AST> & tree)
{
	switch(tree->type){
	case AST::Type::TOP:
		visitTop(tree);
		break;
	case AST::Type::ASSIGNMENT:
		visitAssignment(tree);
		break;
	default:
		std::cerr << "evaluator error" << std::endl;
	}
}