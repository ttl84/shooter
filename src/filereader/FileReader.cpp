#include "FileReader.h"
#include <iostream>
#include <memory>
#include "SyntaxAnalyzer.h"
#include "SemanticAnalyzer.h"
#include "Evaluator.h"
FileReader::FileReader(std::istream & is)
{
	CharStream cs(is);
	TokenStream ts(cs);
	Token tok;
	//while(ts >> tok)
	//	std::cout << tok.row << ' ' << tok.col << ' ' << static_cast<int>(tok.type) << ' ' << tok.lexeme << std::endl;
	std::unique_ptr<AST> tree(parse(ts));
	SemanticAnalyzer().visit(tree);
	Evaluator(data).visit(tree);
}
Object * FileReader::get(std::string key)
{
	auto ref = data.find(key);
	if(ref == data.end())
		return nullptr;
	else
		return ref->second;
}
Object * FileReader::getForType(std::string key, Object::Type type)
{
	auto obj = get(key);
	if(obj == nullptr)
		return nullptr;
	else if(obj->type != type)
		return nullptr;
	else
		return obj;
}
Object * FileReader::getString(std::string key)
{
	return getForType(key, Object::Type::STRING);
}

Object * FileReader::getInteger(std::string key)
{
	return getForType(key, Object::Type::INTEGER);
}
Object * FileReader::getReal(std::string key)
{
	return getForType(key, Object::Type::REAL);
}
Object * FileReader::getBoolean(std::string key)
{
	return getForType(key, Object::Type::BOOLEAN);
}