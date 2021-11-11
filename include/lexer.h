#ifndef SYLVANITE_INCLUDE_LEXER_H_
#define SYLVANITE_INCLUDE_LEXER_H_

#include <istream>
#include <vector>

#include "token.h"

class Lexer {
public:
    Lexer(std::istream& input) : input_(input), lineno_(0), charno_(0) {
        Advance();
    }
 //       Advance();
 //   }

    Token Peek() const;

    Token Advance(); // Advances to next non-comment token
private:
    std::istream& input_;
    int lineno_;
    int charno_;
    Token head_;

    void Error(std::string error);
    void Step(); // Consumes one character + whitespace, updates head_

    void ConsumeWhitespace();
    void ConsumeComment();
};

#endif // SYLVANITE_INCLUDE_LEXER_H_
