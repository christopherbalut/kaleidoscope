#include <cctype>
#include <iostream>

// 1.2
// for lexer, we need to allow our language to have:
// eof (edge case if we are at end of file)
// def, extern, identifier(variable assigned by the programmer),
// the lexer treats 35.1, the same as 47.5, and 31 so we need a number too
// all the '+' '-', etc is taken over by the cpp language itself
// we use enum because we want to use
// IdentifierStr: there can be two identifier's but we need to know which one
// NumVal: same as above
// we don't need to use for def because def can only have one definition (extra
// information)

enum Token {
    tok_eof = -1,
    tok_def = -2,
    tok_extern = -3,
    tok_identifier = -4,
    tok_number = -5,
};

static std::string IdentifierStr; // static because we want to keep it here
static double NumVal;

// gettok():
//  skips any whitespace
//  if not whitespace, add to IdentifierStr, and check if we have def or extern
//  since those are the only two 'fixed' keywords for numbers we check if it has
//  decimal or if the lastchar is a digit, we add to numstr until we cannot
//  anymore, convert and return for comments: we keep moving until we hit eof or
//  new line or \r we check for eof and return or we have an operator

static int gettok() {
    static int LastChar = ' ';

    while (std::isspace(LastChar)) {
        LastChar = getchar();
    }

    if (std::isalpha(LastChar)) {
        IdentifierStr = LastChar;

        // get char() moves pointer foward
        while (std::isalnum(LastChar = std::getchar())) {
            IdentifierStr += LastChar;
        }

        if (IdentifierStr == "def") {
            return tok_def;
        }

        if (IdentifierStr == "extern") {
            return tok_extern;
        }
        return tok_identifier;
    }

    if (std::isdigit(LastChar) || LastChar == '.') {
        // std::cout << "LastChar int:" << LastChar << "\n";
        // std::cout << "LastChar char: " << static_cast<char>(LastChar) <<
        // '\n';
        std::string NumStr;

        while (std::isdigit(LastChar) || LastChar == '.') {
            NumStr += LastChar;
            LastChar = std::getchar();

            //   std::cout << "after getchar():\n";
            //  std::cout << "  int  = " << LastChar << '\n';
            // std::cout << "  char = " << static_cast<char>(LastChar) << '\n';

            // std::cout << "NumStr is " << NumStr << "\n";
        }
        NumVal = std::strtod(NumStr.c_str(), 0);
        // std::cout << "NumVal is " << NumVal << "\n";
        return tok_number;
    }

    if (LastChar == '#') {
        while (LastChar != EOF && LastChar != '\n' && LastChar != '\r') {
            LastChar = std::getchar();
        }

        // if not end of file then we restart from the beginning
        if (LastChar != EOF) {
            return gettok();
        }
    }

    if (LastChar == EOF) {
        return tok_eof;
    }

    int ThisChar = LastChar;
    LastChar = std::getchar();
    return ThisChar;
}

int main() {
    while (true) {
        int Token = gettok();

        if (Token == tok_eof) {
            std::cout << "tok_eof\n";
            break;
        }

        if (Token == tok_def) {
            std::cout << "tok_def\n";
        } else if (Token == tok_extern) {
            std::cout << "tok_extern\n";
        } else if (Token == tok_identifier) {
            std::cout << "tok_identifier: " << IdentifierStr << '\n';
        } else if (Token == tok_number) {
            std::cout << "tok_number: " << NumVal << '\n';
        } else {
            std::cout << "character: " << static_cast<char>(Token) << '\n';
        }
    }
}
