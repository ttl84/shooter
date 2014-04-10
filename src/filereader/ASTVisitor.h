#ifndef ASTVisitor_h
#define ASTVisitor_h
class ASTVisitor{
	typedef void visit_t(std::unique_ptr<AST> & tree);
	virtual visit_t visitTop = 0;
	virtual visit_t visitAssignment = 0;
	virtual visit_t visitIdentifier = 0;
	virtual visit_t visitReal = 0;
	virtual visit_t visitInteger = 0;
	virtual visit_t visitBoolean = 0;
	virtual visit_t visitString = 0;
	virtual visit_t visitList = 0;
	virtual visit_t visitError = 0;
	virtual visit_t visitNone = 0;
public:
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
			
			case AST::Type::NONE:
				visitNone(tree);
				break;
		}
	}
};
#endif
