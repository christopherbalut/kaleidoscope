#include "ast.hpp"
#include "parser.hpp"
#include <map>
#include <utility>

using llvm::Function;
using llvm::Value;

std::unique_ptr<llvm::LLVMContext> TheContext;
std::unique_ptr<llvm::IRBuilder<>> Builder;
std::unique_ptr<llvm::Module> TheModule;
std::map<std::string, Value *>
    NamedValues; // keeps track of current values that are defined in different
                 // scopes
NumberExprAST::NumberExprAST(double Val) : Val(Val) {}

double NumberExprAST::GetValue() const { return Val; } // temp for -Werror

Value *NumberExprAST::codegen() {
    Value *fpconst{llvm::ConstantFP::get(*TheContext, llvm::APFloat(Val))};
    if (!fpconst) {
        return nullptr;
    }
    return fpconst;
}

VariableExprAST::VariableExprAST(const std::string &Name) : Name(Name) {}

Value *VariableExprAST::codegen() {
    Value *V{NamedValues[Name]};

    if (!V) {
        return LogErrorV("Unkown variable name");
    }

    return V;
}

BinaryExprAST::BinaryExprAST(char Op, std::unique_ptr<ExprAST> LHS,
                             std::unique_ptr<ExprAST> RHS)
    : Op(Op), RHS(std::move(RHS)), LHS(std::move(LHS)) {}
// std::move since unique_ptr's cannot be copied

char BinaryExprAST::GetOp() const { return Op; } // temp for -Werror

// case of 1 + 2
// we recurse on the left, so we call
Value *BinaryExprAST::codegen() {
    Value *L{LHS->codegen()};
    Value *R{RHS->codegen()};

    if (!L || !R) {
        return nullptr;
    }

    switch (Op) {
    case '+':
        return Builder->CreateFAdd(L, R, "add");
        break;
    case '-':
        return Builder->CreateFSub(L, R, "sub");
        break;
    case '*':
        return Builder->CreateFMul(L, R, "mult");
        break;
    case '<':
        // floating point comparison creates i32
        L = Builder->CreateFCmpULT(L, R, "cmp");
        return Builder->CreateUIToFP(L, llvm::Type::getDoubleTy(*TheContext));
        break;
    default:
        // i probably have to log the error somewhere
        return LogErrorV("invalid binary operator");
    }
}

CallExprAST::CallExprAST(const std::string &Callee,
                         std::vector<std::unique_ptr<ExprAST>> Args)
    : Callee(Callee), Args(std::move(Args)) {}

Value *CallExprAST::codegen() {
    Function *CalleeF{TheModule->getFunction(Callee)};
    if (!CalleeF) { // function does not exist
        return nullptr;
    }

    if (CalleeF->arg_size() != Args.size()) {
        return LogErrorV("Incorrect number of arguments passed");
    }

    std::vector<Value *> ArgsV;
    for (unsigned i{}; i < Args.size(); i++) {
        ArgsV.emplace_back(Args[i]->codegen());
    }

    if (!ArgsV.back()) { // if most recent arg failed to codegen
        return nullptr;
    }

    return Builder->CreateCall(CalleeF, ArgsV, "call");
}

PrototypeAST::PrototypeAST(const std::string &Name,
                           std::vector<std::string> Args)
    : Name(Name), Args(std::move(Args)) {}

const std::string &PrototypeAST::GetName() const { return Name; }

llvm::Function *PrototypeAST::codegen() {
    std::vector<llvm::Type *> Doubles(
        Args.size(), // creating a vector for the Module
        llvm::Type::getDoubleTy(*TheContext));

    // return type, parameters, not variadic
    llvm::FunctionType *FT{llvm::FunctionType::get(
        llvm::Type::getDoubleTy(*TheContext), Doubles, false)};

    llvm::Function *F{
        Function::Create(FT, Function::ExternalLinkage, Name, TheModule.get())};

    unsigned Idx{0};
    for (auto &Arg : F->args()) {
        Arg.setName(Args[Idx++]);
    }

    return F;
}

FunctionAST::FunctionAST(std::unique_ptr<PrototypeAST> Proto,
                         std::unique_ptr<ExprAST> Body)
    : Proto(std::move(Proto)), Body(std::move(Body)) {}

llvm::Function *FunctionAST::codegen() {
    Function *TheFunction{TheModule->getFunction(Proto->GetName())};

    if (!TheFunction) {
        TheFunction = Proto->codegen();
    }

    if (!TheFunction) {
        return nullptr;
    }

    if (!TheFunction->empty()) {
        return (Function *)LogErrorV("Function cannot be redefined");
    }

    // create a new basic block to start insertion into it
    llvm::BasicBlock *BB{
        llvm::BasicBlock::Create(*TheContext, "entry", TheFunction)};
    Builder->SetInsertPoint(BB);

    // record the function arguments in the NamedValues map
    NamedValues.clear();
    for (auto &Arg : TheFunction->args()) {
        NamedValues[std::string(Arg.getName())] = &Arg;
    }

    //
    if (Value * RetVal{Body->codegen()}) {
        Builder->CreateRet(RetVal);
        llvm::verifyFunction(*TheFunction);

        return TheFunction;
    }

    TheFunction->eraseFromParent();
    return nullptr;
}
