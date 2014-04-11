#ifndef SyntaxAnalyzer_H
#define SyntaxAnalyzer_H
#include "AST.h"
#include "TokenStream.h"
#include <memory>
std::unique_ptr<AST> parse(TokenStream & ts);
#endif
