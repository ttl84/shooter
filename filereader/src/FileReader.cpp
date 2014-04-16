#include "FileReader.h"
#include <iostream>
#include <memory>
#include "TokenStream.h"
#include "SyntaxAnalyzer.h"
#include "SemanticAnalyzer.h"
#include "Evaluator.h"
FileReader::FileReader(std::istream & is)
{
	CharStream cs(is);
	TokenStream ts(cs);
	std::unique_ptr<AST> tree(parse(ts));
	SemanticAnalyzer().visit(tree);
	Evaluator(data, pool).visit(tree);
}
Object * FileReader::get(std::string key)
{
	auto ref = data.find(key);
	if(ref == data.end())
		return nullptr;
	else
		return ref->second;
}
StringObject * FileReader::getString(std::string key)
{
	return downcast<StringObject>(get(key));
}
IntegerObject * FileReader::getInteger(std::string key)
{
	return downcast<IntegerObject>(get(key));
}
RealObject * FileReader::getReal(std::string key)
{
	return downcast<RealObject>(get(key));
}
BooleanObject * FileReader::getBoolean(std::string key)
{
	return downcast<BooleanObject>(get(key));
}
ListObject * FileReader::getList(std::string key)
{
	return downcast<ListObject>(get(key));
}
