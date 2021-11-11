#ifndef SYLVANITE_INCLUDE_TOKEN_H_
#define SYLVANITE_INCLUDE_TOKEN_H_

#include <optional>

// We can easily map characters 1-to-1 with tokens because Brainfuck only has
// single-character operators: perhaps not best practice, but works fine here.
enum class Token : char {
    Invalid  = 0,
    Eof      = EOF,
    Plus     = '+',
    Minus    = '-',
    Period   = '.',
    Comma    = ',',
    ChevronR = '>',
    ChevronL = '<',
    BraceR   = ']',
    BraceL   = '[',
};

bool isValidToken(char c);

bool isValidToken(Token t);

Token charToToken(char c);

char tokenToChar(Token t);

#endif // SYLVANITE_INCLUDE_TOKEN_H_
