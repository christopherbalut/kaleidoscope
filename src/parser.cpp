#include "parser.hpp"
#include "lexer.hpp"
#include <memory>
#include <vector>

int CurTok;
std::map<char, int> BinopPrecedence;
// read token from the lexer and updates the current token

int GetNextToken() { return CurTok = gettok(); }

std::unique_ptr<ExprAST> LogError(const char *Str) {
    fprintf(stderr, "Error: %s\n", Str);
    return nullptr;
}
std::unique_ptr<PrototypeAST> LogErrorP(const char *Str) {
    LogError(Str);
    return nullptr;
}

llvm::Value *LogErrorV(const char *Str) {
    LogError(Str);
    return nullptr;
}

std::unique_ptr<ExprAST> ParseNumberExpr() {
    auto Result = std::make_unique<NumberExprAST>(NumVal);
    GetNextToken();
    return std::move(Result);
}

std::unique_ptr<ExprAST> ParseParenExpr() {
    GetNextToken();

    auto V = ParseExpression();
    if (!V) {
        return nullptr;
    }

    if (CurTok != ')') {
        return LogError("Expecting closing ')', terminating...");
    }

    GetNextToken();
    return V;
}

std::unique_ptr<ExprAST> ParseIdentifierExpr() {
    std::string IdName{IdentifierStr};

    GetNextToken();

    if (CurTok != '(') {
        return std::make_unique<VariableExprAST>(IdName);
    }

    GetNextToken(); // for '('
    std::vector<std::unique_ptr<ExprAST>> Args;

    if (CurTok != ')') {
        while (true) {
            if (auto Arg{ParseExpression()}) {
                Args.emplace_back(std::move(Arg));
            } else {
                return nullptr;
            }

            if (CurTok == ')') {
                break;
            }

            if (CurTok != ',') {
                return LogError("Expecting comma to seperate function input "
                                "parameters, terminating...");
            }
            GetNextToken();
        }
    }

    GetNextToken(); // for ')'
    return std::make_unique<CallExprAST>(IdName, std::move(Args));
}

std::unique_ptr<ExprAST> ParsePrimary() {
    switch (CurTok) {
    default:
        return LogError(
            "Unknown token when expecting an expression, terminating...");
    case tok_identifier:
        return ParseIdentifierExpr();
    case tok_number:
        return ParseNumberExpr();
    case '(':
        return ParseParenExpr();
    }
}

int GetTokPrecedence() {
    if (!isascii(CurTok)) {
        return -1;
    }

    int TokPrec = BinopPrecedence[CurTok];

    if (TokPrec <= 0) {
        return -1;
    }
    return TokPrec;
}

std::unique_ptr<ExprAST> ParseExpression() {

    auto LHS = ParsePrimary();
    if (!LHS) {
        return nullptr;
    }

    return ParseBinOpRHS(0, std::move(LHS));
}

std::unique_ptr<ExprAST> ParseBinOpRHS(int ExprPrec,
                                       std::unique_ptr<ExprAST> LHS) {
    while (true) {
        int TokPrec{GetTokPrecedence()};

        if (TokPrec < ExprPrec) {
            return LHS;
        }

        int BinOp{CurTok};
        GetNextToken();

        auto RHS{ParsePrimary()};
        if (!RHS) {
            return nullptr;
        }

        int NextPrec{GetTokPrecedence()};
        if (TokPrec < NextPrec) {
            RHS = ParseBinOpRHS(TokPrec + 1, std::move(RHS));
            if (!RHS) {
                return nullptr;
            }
        }
        LHS = std::make_unique<BinaryExprAST>(BinOp, std::move(LHS),
                                              std::move(RHS));
    }
}

std::unique_ptr<PrototypeAST> ParsePrototype() {
    if (CurTok != tok_identifier) {
        return LogErrorP("Expected function name in prototype");
    }

    std::string FnName{IdentifierStr};
    GetNextToken();

    if (CurTok != '(') {
        return LogErrorP("Expected open parentheses");
    }

    std::vector<std::string> ArgNames;
    while (GetNextToken() == tok_identifier) {
        ArgNames.emplace_back(IdentifierStr);
    }

    if (CurTok != ')') {
        return LogErrorP("Expected a closing parentheses");
    }

    GetNextToken();

    return std::make_unique<PrototypeAST>(FnName, std::move(ArgNames));
}

std::unique_ptr<FunctionAST> ParseDefinition() {
    GetNextToken();
    auto Proto{ParsePrototype()};
    if (!Proto) {
        return nullptr;
    }

    if (auto E{ParseExpression()}) {
        return std::make_unique<FunctionAST>(std::move(Proto), std::move(E));
    }
    return nullptr;
}

std::unique_ptr<PrototypeAST> ParseExtern() {
    GetNextToken();
    return ParsePrototype();
}

std::unique_ptr<FunctionAST> ParseTopLevelExpr() {
    if (auto E{ParseExpression()}) {
        auto Proto{std::make_unique<PrototypeAST>("__anon_expr",
                                                  std::vector<std::string>())};
        return std::make_unique<FunctionAST>(std::move(Proto), std::move(E));
    }
    return nullptr;
}
