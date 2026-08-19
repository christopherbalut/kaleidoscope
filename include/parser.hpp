#pragma once

#include "ast.hpp"
#include <memory>
int GetNextToken();

std::unique_ptr<ExprAST> LogError(const char *Str);
std::unique_ptr<PrototypeAST> LogErrorP(const char *Str);

std::unique_ptr<ExprAST> ParseNumberExpr();
std::unique_ptr<ExprAST> ParseParenExpr();

std::unique_ptr<ExprAST> ParseExpression();
