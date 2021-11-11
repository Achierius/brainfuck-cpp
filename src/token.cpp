#include "token.h"

#include <assert.h>

#include <algorithm>

Token tokens[] = { Token::Minus, Token::Plus, Token::Period, Token::Comma,
                   Token::ChevronR, Token::ChevronL,
                   Token::BraceR, Token::BraceL};

bool isValidToken(char c) {
    Token t = static_cast<Token>(c);
    return isValidToken(t);
}

bool isValidToken(Token t) {
    return std::find(std::begin(tokens), std::end(tokens), t) !=
           std::end(tokens);
}

Token charToToken(char c) {
    if (c == EOF) return Token::Eof;

    Token t = static_cast<Token>(c);
    return isValidToken(t) ? t : Token::Invalid;
}

char tokenToChar(Token t) {
    assert(t != Token::Invalid);
    assert(t != Token::Eof);

    return static_cast<char>(t);
}
