#include "Evaluator.h"
#include <iostream>
static std::ostream & logError(std::unique_ptr<AST> & tree)
{
	return std::cerr << "evaluator (" << tree->start.row << ", " << tree->start.col << "): ";
}
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
	auto obj = new Object(Object::Type::BOOLEAN);
	obj->datum.boolean = tree->start.datum.boolean;
	objectTable.emplace(tree.get(), obj);
}
void Evaluator::visitInteger(std::unique_ptr<AST> & tree)
{
	auto obj = new Object(Object::Type::INTEGER);
	obj->datum.integer = tree->start.datum.integer;
	objectTable.emplace(tree.get(), obj);
}
void Evaluator::visitReal(std::unique_ptr<AST> & tree)
{
	auto obj = new Object(Object::Type::REAL);
	obj->datum.real = tree->start.datum.real;
	objectTable.emplace(tree.get(), obj);
}
void Evaluator::visitString(std::unique_ptr<AST> & tree)
{
	auto obj = new Object(Object::Type::STRING);
	obj->datum.string = new std::string(tree->start.lexeme);
	objectTable.emplace(tree.get(), obj);
}
void Evaluator::visitList(std::unique_ptr<AST> & tree)
{
	auto obj = new Object(Object::Type::LIST);
	auto list = new std::vector<Object *>;
	for(auto & child : *tree)
	{
		visit(child);
		Object * childObj = getAndRemove(objectTable, child.get());
		list->push_back(childObj);
	}
	obj->datum.list = list;
	objectTable.emplace(tree.get(), obj);
}
void Evaluator::visitIdentifier(std::unique_ptr<AST> & tree)
{
	auto obj = data.at(tree->start.lexeme);
	// increase reference count
	Object::inc(obj);
	objectTable.emplace(tree.get(), obj);
}

void Evaluator::visitAssignment(std::unique_ptr<AST> & tree)
{
	auto & left = tree->child(0);
	auto & right = tree->child(1);
	visit(right);
	if(left->type != AST::Type::ERROR and right->type != AST::Type::ERROR)
	{
		// if identifier is already bound to a value, decrement ref count of the value
		auto old = data.find(left->start.lexeme);
		if(old != data.end())
		{
			Object::dec(old->second);
			data.erase(old);
		}
		auto val = getAndRemove(objectTable, right.get());
		data.emplace(left->start.lexeme, val);
	}
}