#ifndef SyntaxAnalyzer_H
#define SyntaxAnalyzer_H
#include "AST.h"
#include "TokenStream.h"
AST * parse(TokenStream & ts);
#endif
