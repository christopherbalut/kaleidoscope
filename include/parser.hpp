#pragma once

#include "ast.hpp"
#include <map>
#include <memory>

extern int CurTok;
extern std::map<char, int> BinopPrecedence;
int GetNextToken();

std::unique_ptr<ExprAST> LogError(const char *Str);
std::unique_ptr<PrototypeAST> LogErrorP(const char *Str);
llvm::Value *LogErrorV(const char *Str);

std::unique_ptr<ExprAST> ParseNumberExpr();
std::unique_ptr<ExprAST> ParseParenExpr();
std::unique_ptr<ExprAST> ParseIdentifierExpr();

std::unique_ptr<ExprAST> ParsePrimary();

std::unique_ptr<ExprAST> ParseExpression();

extern std::map<char, int> BinopPrecedence;

int GetTokPrecedence();
std::unique_ptr<ExprAST> ParseIdentifierExpr();

std::unique_ptr<ExprAST> ParseBinOpRHS(int ExprPrec,
                                       std::unique_ptr<ExprAST> LHS);

std::unique_ptr<PrototypeAST> ParsePrototype();
std::unique_ptr<FunctionAST> ParseDefinition();
std::unique_ptr<PrototypeAST> ParseExtern();
std::unique_ptr<FunctionAST> ParseTopLevelExpr();
