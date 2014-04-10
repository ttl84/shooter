#ifndef SyntaxAnalyzer_H
#define SyntaxAnalyzer_H
#include "AbstractSyntaxTree.h"
#include "TokenStream.h"
AST * parse(TokenStream & ts);
#endif
