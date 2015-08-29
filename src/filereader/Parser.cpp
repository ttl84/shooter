#include "Parser.h"
#include "Object.h"
#include "TokenStream.h"
#include <algorithm>
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <stack>
#include <cassert>
#include <iostream>

namespace fr{
namespace NonTerminal{
	typedef Token::Type Type;
	static const std::vector<Type> EXPR{Type::INTEGER, Type::REAL, Type::BOOLEAN, Type::STRING, Type::IDENTIFIER, Type::BEGIN_LIST};
} // end namespace fr
class Parser{
	
	struct StackItem{
		enum class Type: uint8_t{
			ERROR, START_LIST, OBJECT
		} type;
		Object * obj;
	};

	std::stack<StackItem> stack;
	TokenStream & ts;
	ObjectTable & data;
	ObjectPool & pool;

	// top -> statement*
	void top()
	{
		while(not accept(Token::Type::END))
			statement();
	}
	// statement -> assignment*
	void statement()
	{
		assignment();
	}
	// assignment -> identifier := expr {Assign(identifier)}
	void assignment()
	{
		auto id = ts.get();
		match(Token::Type::ASSIGNMENT);
		expr();
		Assign(id);
	}
	//expr -> list | iden | int | real | string | bool
	void expr()
	{
		if(accept(Token::Type::BEGIN_LIST))
			list();
		else if(accept(Token::Type::IDENTIFIER))
			identifier();
		else if(accept(Token::Type::INTEGER))
			integer();
		else if(accept(Token::Type::REAL))
			real();
		else if(accept(Token::Type::STRING))
			string();
		else if(accept(Token::Type::BOOLEAN))
			boolean();
		else
		{
			push({StackItem::Type::ERROR, nullptr});
			error("any expressions");
			panic(NonTerminal::EXPR);
		}
	}
	// list -> '[' commaList ']' {
	void list()
	{
		match(Token::Type::BEGIN_LIST);
		push({StackItem::Type::START_LIST, nullptr});
		commaList();
		match(Token::Type::END_LIST);
		MakeList();
	}
	// commaList -> epsilon | expr (, expr)*
	void commaList()
	{
		if(not accept(NonTerminal::EXPR))
			return;
		expr();
		while(accept(Token::Type::CONTINUE_LIST))
		{
			ts.get();
			expr();
		}
	}
	//identifier -> {push(identifier.val)}
	void identifier()
	{
		auto it = data.find(ts.get().lexeme);
		if(it == data.end())
			push({StackItem::Type::ERROR, nullptr});
		else
			push({StackItem::Type::OBJECT, it->second});
	}
	// integer -> {push(integer.val)}
	void integer()
	{
		auto obj = new IntegerObject();
		pool.emplace_back(obj);
		obj->datum = ts.get().datum.integer;
		push({StackItem::Type::OBJECT, obj});
	}
	// real -> {push(real.val)}
	void real()
	{
		auto obj = new RealObject();
		pool.emplace_back(obj);
		obj->datum = ts.get().datum.real;
		push({StackItem::Type::OBJECT, obj});
	}
	// string -> {push(string.val)}
	void string()
	{
		auto obj = new StringObject();
		pool.emplace_back(obj);
		obj->datum = ts.get().lexeme;
		push({StackItem::Type::OBJECT, obj});
	}
	// bool -> {push(bool.val)}
	void boolean()
	{
		auto obj = new BooleanObject();
		pool.emplace_back(obj);
		obj->datum = ts.get().datum.boolean;
		push({StackItem::Type::OBJECT, obj});
	}
	// actions
	void Assign(Token & tok)
	{
		auto expr = pop();
		if(expr.type == StackItem::Type::OBJECT)
			data[tok.lexeme] = expr.obj;
	}
	void MakeList()
	{
		std::vector<Object*> v;
		bool err = false;
		while(true)
		{
			auto expr = pop();
			if(expr.type == StackItem::Type::START_LIST)
				break;
			else if(expr.type == StackItem::Type::OBJECT)
				v.push_back(expr.obj);
			else if(expr.type == StackItem::Type::ERROR)
				err = true;
		}
		if(err)
		{
			push({StackItem::Type::ERROR, nullptr});
			stack_error();
		}
		else
		{
			std::reverse(v.begin(), v.end());
			auto obj = new ListObject();
			pool.emplace_back(obj);
			obj->datum = v;
			push({StackItem::Type::OBJECT, obj});
		}
	}

	void push(StackItem item)
	{
		stack.push(item);
	}
	StackItem pop()
	{
		assert(not stack.empty());
		auto item = stack.top();
		stack.pop();
		return item;
	}
	
	bool accept(Token::Type type)
	{
		return ts.peek().type == type;
	}
	bool accept(std::vector<Token::Type> const & types)
	{
		auto & tok = ts.peek();
		for(auto type : types)
			if(tok.type == type)
				return true;
		return false;
	}
	template<class T>
	bool match(T args)
	{
		if(accept(args))
		{
			ts.get();
			return true;
		}
		else
		{
			error(args);
			return false;
		}
	}
	template<class T>
	void panic(T args)
	{
		ts.get();
		while(not accept(args) and not accept(Token::Type::END))
			ts.get();
	}
	template<class T>
	void error(T thing)
	{
		auto & tok = ts.peek();
		std::cerr << __func__ << "(" << tok.row << ", " << tok.col << "): " << "expecting " << thing << ", got " << tok << std::endl;
	}
	void stack_error()
	{
		std::cerr << __func__<< "encountered error in stack. continuing" << std::endl;
	}
public:
	Parser(TokenStream & stream, ObjectTable & objectTable, ObjectPool & objectPool)
		:ts(stream), data(objectTable), pool(objectPool){}
	void run()
	{
		top();
	}
};

void parse(TokenStream & ts, ObjectTable & table, ObjectPool & pool)
{
	Parser(ts, table, pool).run();
}
} // end namespace fr
