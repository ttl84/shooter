#include "SemanticAnalyzer.h"
#include <iostream>
std::ostream & logError(std::unique_ptr<AST> & tree)
{
	return std::cerr << "at " << tree->start.row << ", " << tree->start.col << ": ";
}
void SemanticAnalyzer::visitError(std::unique_ptr<AST> & tree)
{
	
}
void SemanticAnalyzer::visitAssignment(std::unique_ptr<AST> & tree)
{
	auto & left = tree->children[0];
	auto & right = tree->children[1];
	if(data.find(left->start.lexeme) != data.end())
	{
		logError(tree) << "identifier already defined: [" << left->start.lexeme << ']' << std::endl;
	}
	else if(right->type == AST::Type::IDENTIFIER)
	{
		auto ref = data.find(right->start.lexeme);
		if(ref != data.end())
			data[left->start.lexeme] = data[right->start.lexeme];
		else
			logError(tree) << "unbound identifier: [" << right->start.lexeme << ']' << std::endl;
	}
	else
	{
		data[left->start.lexeme] = right->start;
	}
}
void SemanticAnalyzer::visitTop(std::unique_ptr<AST> & tree)
{
	for(std::unique_ptr<AST> & child : tree->children)
		visit(child);
}
void SemanticAnalyzer::visit(std::unique_ptr<AST> & tree)
{
	switch(tree->type){
	case AST::Type::TOP:
		visitTop(tree);
		break;
	case AST::Type::ASSIGNMENT:
		return visitAssignment(tree);
		break;
	default:
		logError(tree) << "visit for tree type not implemented" << std::endl;
	}
}
SemanticAnalyzer::SemanticAnalyzer(std::map<std::string, Token> & dataMap)
:data(dataMap)
{
	
}