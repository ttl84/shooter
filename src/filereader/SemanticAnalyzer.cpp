#include "SemanticAnalyzer.h"
#include <iostream>
std::ostream & logError(std::unique_ptr<AST> & tree)
{
	return std::cerr << "semantic analyzer (" << tree->start.row << ", " << tree->start.col << "): ";
}
bool SemanticAnalyzer::bad(void)
{
	return badFlag;
}
void SemanticAnalyzer::semanticError(void)
{
	badFlag = true;
}
void SemanticAnalyzer::visitError(std::unique_ptr<AST> & tree)
{
	semanticError();
}
void SemanticAnalyzer::visitIdentifier(std::unique_ptr<AST> & tree)
{
	if(parent->type == AST::Type::ASSIGNMENT and tree == parent->children.at(0))
	{
		symbolTable.insert(tree->start->lexeme);
	}
	else
	{
		auto ref = symbolTable.find(tree->start.lexeme);
		if(ref == data.end())
		{
			logError(tree) << "unbound identifier: [" << tree->start.lexeme << ']' << std::endl;
			semanticError();
		}
	}
}
void SemanticAnalyzer::visitAssignment(std::unique_ptr<AST> & tree)
{
	auto & left = tree->children.at(0);
	auto & right = tree->children.at(1);
	visit(left, tree);
	visit(right, tree);
	if(right->type == AST::Type::IDENTIFIER)
	{
		
	}
}
void SemanticAnalyzer::visitList(std::unique_ptr<AST> & tree)
{
	for(auto & child : tree->children)
		visit(child, tree);
}
void SemanticAnalyzer::visitTop(std::unique_ptr<AST> & tree)
{
	for(auto & child : tree->children)
		visit(child, tree);
}
