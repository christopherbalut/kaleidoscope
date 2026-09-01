#pragma once
#include "llvm/ADT/APFloat.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Verifier.h"
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Type.h>
#include <llvm/Support/Casting.h>
#include <memory>
#include <string>
#include <vector>

extern std::unique_ptr<llvm::LLVMContext>
    TheContext; // owns different core LLVM data structures like types and
                // constant values
extern std::unique_ptr<llvm::IRBuilder<>>
    Builder; // helper object to generate LLVM instructions
extern std::unique_ptr<llvm::Module>
    TheModule; // constains functions and global variables

// we implement a class with for the AST since each kind of syntax is
// represented by a different C++ class
// The language has: double NumVal, string Variables, Binary Expression, and
// function calls
// to handle resources, the expression AST has a destructor to ensure lifetimes
// are handled after recursion

class ExprAST { // all other AST Classes follow from this
  public:
    virtual ~ExprAST() = default;       // destructor to handle lifetimes
    virtual llvm::Value *codegen() = 0; // turning AST into IR
};

// stores literal
class NumberExprAST : public ExprAST {
    double Val; // private by default since it is before public

  public:
    NumberExprAST(double Val);
    double GetValue() const; // temp for -Werror
    llvm::Value *codegen() override;
};

class VariableExprAST : public ExprAST {
    std::string Name;

  public:
    VariableExprAST(const std::string &Name);
    llvm::Value *codegen() override;
};

class BinaryExprAST : public ExprAST {
    char Op;
    std::unique_ptr<ExprAST> RHS, LHS;
    // unique to handle different data types and
    // different data types sizes i.e. object slicing

  public:
    BinaryExprAST(char Op, std::unique_ptr<ExprAST> RHS,
                  std::unique_ptr<ExprAST> LHS);

    char GetOp() const; // temp for -Werror
    llvm::Value *codegen() override;
};

class CallExprAST : public ExprAST {
    std::string Callee;
    std::vector<std::unique_ptr<ExprAST>> Args;

  public:
    CallExprAST(const std::string &Callee,
                std::vector<std::unique_ptr<ExprAST>>);
    llvm::Value *codegen() override;
};

// we use PrototypeAST for function headers: string for the function name
// and a vector for the input arguments
// and FunctionAST for the header (PrototypeAST for the header)
// and body (an entire expression)

class PrototypeAST {
    std::string Name;
    std::vector<std::string> Args;

  public:
    PrototypeAST(const std::string &Name, std::vector<std::string> Args);

    const std::string &GetName() const;
    llvm::Function *codegen();
};

class FunctionAST {
    std::unique_ptr<PrototypeAST> Proto;
    std::unique_ptr<ExprAST> Body;

  public:
    FunctionAST(std::unique_ptr<PrototypeAST> Proto,
                std::unique_ptr<ExprAST> Body);

    llvm::Function *codegen();
};
