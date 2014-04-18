#ifndef ASTVisitor_h
#define ASTVisitor_h
#include "AST.h"
#include <iostream>
class ASTVisitor{
private:
	#define DECL_VISIT(name)\
	virtual void name (std::unique_ptr<AST> & tree)\
	{\
		for(auto & child : *tree)\
			visit(child);\
	}
	
	DECL_VISIT(visitTop)
	DECL_VISIT(visitAssignment)
	DECL_VISIT(visitIdentifier)
	DECL_VISIT(visitReal)
	DECL_VISIT(visitInteger)
	DECL_VISIT(visitBoolean)
	DECL_VISIT(visitString)
	DECL_VISIT(visitList)
	DECL_VISIT(visitError)
	#undef DECL_VISIT
	
	// return a string that describes the visitor
	virtual std::string getName() const = 0;
protected:
	std::ostream & logError(std::unique_ptr<AST> & tree) const
	{
		return std::cerr <<
			getName() <<
			" (" << tree->start.row << ", " << tree->start.col << "): ";
	}
public:
	virtual ~ASTVisitor(){}
	void visit(std::unique_ptr<AST> & tree)
	{
		switch(tree->type){
			case AST::Type::TOP:
				visitTop(tree);
				break;
			
			case AST::Type::ASSIGNMENT:
				visitAssignment(tree);
				break;
			
			case AST::Type::IDENTIFIER:
				visitIdentifier(tree);
				break;
			
			case AST::Type::REAL:
				visitReal(tree);
				break;
			case AST::Type::BOOLEAN:
				visitBoolean(tree);
				break;
			
			case AST::Type::INTEGER:
				visitInteger(tree);
				break;
			
			case AST::Type::STRING:
				visitString(tree);
				break;
			
			case AST::Type::LIST:
				visitList(tree);
				break;
			
			case AST::Type::ERROR:
				visitError(tree);
				break;
			
		}
	}

};
#endif
