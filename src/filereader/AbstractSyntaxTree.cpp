#include "AbstractSyntaxTree.h"
void AST::append(AST * child)
{
	child->parent = this;
	children.emplace_back(child);
}
AST * AST::getParent(void)
{
	return parent;
}
std::unique_ptr<AST> & AST::at(unsigned i)
{
	return children.at(i);
}

AST::iterator begin(AST const & tree)
{
	return tree.children.begin();
}
AST::iterator end(AST const & tree)
{
	return tree.children.end();
}
