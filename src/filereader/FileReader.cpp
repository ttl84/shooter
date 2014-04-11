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
	Token tok;
	//while(ts >> tok)
	//	std::cout << tok.row << ' ' << tok.col << ' ' << static_cast<int>(tok.type) << ' ' << tok.lexeme << std::endl;
	std::unique_ptr<AST> tree(parse(ts));
	SemanticAnalyzer().visit(tree);
	Evaluator(data).visit(tree);
}
std::shared_ptr<Object> FileReader::get(std::string key)
{
	auto ref = data.find(key);
	if(ref == data.end())
		return nullptr;
	else
		return ref->second;
}
std::shared_ptr<Object> FileReader::getForType(std::string key, Object::Type type)
{
	auto obj = get(key);
	if(obj == nullptr)
		return nullptr;
	else if(obj->type != type)
		return nullptr;
	else
		return obj;
}
std::shared_ptr<Object> FileReader::getString(std::string key)
{
	return getForType(key, Object::Type::STRING);
}

std::shared_ptr<Object> FileReader::getInteger(std::string key)
{
	return getForType(key, Object::Type::INTEGER);
}
std::shared_ptr<Object> FileReader::getReal(std::string key)
{
	return getForType(key, Object::Type::REAL);
}
std::shared_ptr<Object> FileReader::getBoolean(std::string key)
{
	return getForType(key, Object::Type::BOOLEAN);
}
std::shared_ptr<Object> FileReader::getList(std::string key)
{
	return getForType(key, Object::Type::LIST);
}
