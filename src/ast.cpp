#include "ast.hpp"

#include <utility>

NumberExprAST::NumberExprAST(double Val) : Val(Val) {}

double NumberExprAST::GetValue() const { return Val; } // temp for -Werror

VariableExprAST::VariableExprAST(const std::string &Name) : Name(Name) {}

BinaryExprAST::BinaryExprAST(char Op, std::unique_ptr<ExprAST> LHS,
                             std::unique_ptr<ExprAST> RHS)
    : Op(Op), RHS(std::move(RHS)), LHS(std::move(LHS)) {}
// std::move since unique_ptr's cannot be copied

char BinaryExprAST::GetOp() const { return Op; } // temp for -Werror

CallExprAST::CallExprAST(const std::string &Callee,
                         std::vector<std::unique_ptr<ExprAST>> Args)
    : Callee(Callee), Args(std::move(Args)) {}

PrototypeAST::PrototypeAST(const std::string &Name,
                           std::vector<std::string> Args)
    : Name(Name), Args(std::move(Args)) {}

const std::string &PrototypeAST::GetName() const { return Name; }

FunctionAST::FunctionAST(std::unique_ptr<PrototypeAST> Proto,
                         std::unique_ptr<ExprAST> Body)
    : Proto(std::move(Proto)), Body(std::move(Body)) {}
