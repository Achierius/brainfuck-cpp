#ifndef SYLVANITE_INCLUDE_DEBUG_H_
#define SYLVANITE_INCLUDE_DEBUG_H_

#include <vector>
#include <istream>

#include "token.h"
#include "ast.h"

void PrintTokens(std::vector<Token>& tokens);
void PrintStates(std::vector<AtomicState>& states);

void InterpretStates(std::vector<AtomicState>& states, std::istream& input);

#endif // SYLVANITE_INCLUDE_DEBUG_H_
