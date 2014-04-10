#include "Evaluator.h"
#include <iostream>
void Evaluator::visitBoolean(std::unique_ptr<AST> & tree)
{
	auto obj = new Object{Object::Type::BOOLEAN};
	obj->datum.boolean = tree->start.datum.boolean;
	objectTable.emplace(tree.get(), obj);
}
void Evaluator::visitInteger(std::unique_ptr<AST> & tree)
{
	auto obj = new Object{Object::Type::INTEGER};
	obj->datum.integer = tree->start.datum.integer;
	objectTable.emplace(tree.get(), obj);
}
void Evaluator::visitReal(std::unique_ptr<AST> & tree)
{
	auto obj = new Object{Object::Type::REAL};
	obj->datum.real = tree->start.datum.real;
	objectTable.emplace(tree.get(), obj);
}
void Evaluator::visitString(std::unique_ptr<AST> & tree)
{
	auto obj = new Object{Object::Type::STRING};
	obj->datum.string = new std::string(tree->start.lexeme);
	objectTable.emplace(tree.get(), obj);
}
void Evaluator::visitList(std::unique_ptr<AST> & tree)
{
	auto obj = new Object{Object::Type::LIST};
	obj->datum.list = new std::vector<Object *>;
	for(auto & child : *tree)
	{
		visit(child);
		Object * childObj = objectTable.at(child.get());
		obj->datum.list->push_back(childObj);
	}
	objectTable.emplace(tree.get(), obj);
}
void Evaluator::visitIdentifier(std::unique_ptr<AST> & tree)
{
	auto obj = data.at(tree->start.lexeme);
	objectTable.emplace(tree.get(), obj);
}

void Evaluator::visitAssignment(std::unique_ptr<AST> & tree)
{
	auto & left = tree->child(0);
	auto & right = tree->child(1);
	visit(left);
	visit(right);
	data[left->start.lexeme] = objectTable.at(right.get());
}
void Evaluator::visitTop(std::unique_ptr<AST> & tree)
{
	for(auto & child : *tree)
		visit(child);
}