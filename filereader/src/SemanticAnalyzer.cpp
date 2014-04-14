#include "SemanticAnalyzer.h"
#include <iostream>
static std::ostream & logError(std::unique_ptr<AST> & tree)
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
	logError(tree) << "error node" << std::endl;
	semanticError();
}
void SemanticAnalyzer::visitIdentifier(std::unique_ptr<AST> & tree)
{
	auto parent = tree->getParent();
	if(parent->type == AST::Type::ASSIGNMENT and tree == parent->child(0))
	{
		symbolTable.insert(tree->start.lexeme);
	}
	else
	{
		auto ref = symbolTable.find(tree->start.lexeme);
		if(ref == symbolTable.end())
		{
			logError(tree) << "unbound identifier: [" << tree->start.lexeme << ']' << std::endl;
			semanticError();
		}
	}
}

