#include "AST.h"
void AST::append(AST * child)
{
	child->parent = this;
	children.emplace_back(child);
}
AST * AST::getParent(void)
{
	return parent;
}
std::unique_ptr<AST> & AST::child(unsigned i)
{
	return children.at(i);
}

