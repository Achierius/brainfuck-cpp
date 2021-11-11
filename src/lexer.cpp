#include "lexer.h"

#include <cstdio>
#include <cctype>
#include <cassert>

#include <iostream>

Token Lexer::Peek() const {
    return head_;
}

Token Lexer::Advance() {
    Token retval = head_;
    Step();
    while (head_ == Token::Invalid) {
        Step();
        ConsumeWhitespace();
    }
    return retval;
}

void Lexer::Error(std::string error) {
    std::cerr << "Error at line " << lineno_ << ", character " << charno_
              << ": " << error;
    assert(!"error reached"); // idk todo error state
}

void Lexer::ConsumeWhitespace() {
    // We consume whitespace manually (instead of using std::ws) so we can keep
    // track of our position within the stream
    while (std::isspace(input_.peek())) {
        char c = input_.get();
        if (c == '\n') {
            lineno_++;
            charno_ = 0;
        } else {
            charno_++;
        }
    }
}

void Lexer::Step() {
    head_ = charToToken(input_.get());
    this->ConsumeWhitespace();
}
