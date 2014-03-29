#include "FileReader.h"
#include <iostream>
#include <memory>
#include "SyntaxAnalyzer.h"
void visitTop(std::unique_ptr<AST> & tree)
{
	for(std::unique_ptr<AST> & child : tree.children)
		visit(child);
}
void visit(std::unique_ptr<AST> & tree)
{
	switch(tree->type){
	case AST::Type::TOP:
		visitTop(tree);
		break;
	case AST::Type::ASSIGNMENT:
		return visitAssignment(tree);
		break;
	default:
		visitError(tree);
	}
}
FileReader::FileReader(std::istream & is)
{
	CharStream cs(is);
	TokenStream ts(cs);
	std::unique_ptr<AST> tree(parse(ts));
	
}