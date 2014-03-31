#include "FileReader.h"
#include <iostream>
#include <memory>
#include "SyntaxAnalyzer.h"
#include "SemanticAnalyzer.h"
FileReader::FileReader(std::istream & is)
{
	CharStream cs(is);
	TokenStream ts(cs);
	std::unique_ptr<AST> tree(parse(ts));
	SemanticAnalyzer(data).visit(tree);
	
}