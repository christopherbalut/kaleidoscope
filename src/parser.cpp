#include "parser.hpp"
#include "lexer.hpp"
#include <memory>
#include <vector>

using std::unique_ptr;

int CurTok; // Current token we are looking at

// read token from the lexer and updates the current toke
int GetNextToken() { return CurTok = gettok(); }

std::unique_ptr<ExprAST> LogError(const char *Str) {
    fprintf(stderr, "Error: %s\n", Str);
    return nullptr;
}
std::unique_ptr<PrototypeAST> LogErrorP(const char *Str) {
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
