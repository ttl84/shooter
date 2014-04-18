#include "Evaluator.h"
#include <iostream>

template<typename key_t, typename val_t>
static val_t getAndRemove(std::map<key_t, val_t> & map, key_t key)
{
	val_t val = map.at(key);
	map.erase(key);
	return val;
}
void Evaluator::visitError(std::unique_ptr<AST> & tree)
{
	logError(tree) << "error node" << std::endl;
}
void Evaluator::visitBoolean(std::unique_ptr<AST> & tree)
{
	auto obj = new BooleanObject;
	pool.emplace_back(obj);
	obj->datum = tree->start.datum.boolean;
	objectTable[tree.get()] = obj;
}
void Evaluator::visitInteger(std::unique_ptr<AST> & tree)
{
	auto obj = new IntegerObject;
	pool.emplace_back(obj);
	obj->datum = tree->start.datum.integer;
	objectTable[tree.get()] = obj;
}
void Evaluator::visitReal(std::unique_ptr<AST> & tree)
{
	auto obj = new RealObject;
	pool.emplace_back(obj);
	obj->datum = tree->start.datum.real;
	objectTable[tree.get()] = obj;
}
void Evaluator::visitString(std::unique_ptr<AST> & tree)
{
	auto obj = new StringObject;
	pool.emplace_back(obj);
	obj->datum = tree->start.lexeme;
	objectTable[tree.get()] = obj;
}
void Evaluator::visitList(std::unique_ptr<AST> & tree)
{
	auto obj = new ListObject;
	pool.emplace_back(obj);
	for(auto & child : *tree)
	{
		visit(child);
		auto childObj = getAndRemove(objectTable, child.get());
		obj->datum.push_back(childObj);
	}
	objectTable[tree.get()] = obj;
}
void Evaluator::visitIdentifier(std::unique_ptr<AST> & tree)
{
	auto obj = data.at(tree->start.lexeme);
	objectTable[tree.get()] = obj;
}

void Evaluator::visitAssignment(std::unique_ptr<AST> & tree)
{
	auto & left = tree->child(0);
	auto & right = tree->child(1);
	visit(right);
	if(left->type != AST::Type::ERROR and right->type != AST::Type::ERROR)
	{
		auto val = getAndRemove(objectTable, right.get());
		data[left->start.lexeme] = val;
	}
}