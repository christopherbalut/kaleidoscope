#include "lexer.hpp"
#include <iostream>

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
        } else if (Token == tok_error) {
            std::cout << "tok_error\n";
            break;
        } else {
            std::cout << "character: " << static_cast<char>(Token) << '\n';
        }
    }
    return 0;
}
