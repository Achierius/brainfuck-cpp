#include "parser.h"

#include <cassert>
#include <cstdio>

#include <algorithm>
#include <iostream>
#include <exception>
#include <limits>

#include "ast.h"

std::vector<Token> AtomicStateParser::DriveLexer(Lexer& lexer) {
    std::vector<Token> toks;
    Token tok;

    do {
        tok = lexer.Advance();
        toks.push_back(tok);
    } while (tok != Token::Eof);

    return toks;
}

std::vector<AtomicState> AtomicStateParser::ParseAtomicStates() {
    std::vector<AtomicState> states;

    for ( ; ; ) {
        AtomicState new_state;
        switch (driver_.Peek()) {
          case Token::Invalid:
            driver_.ParseError("Invalid token made it through the lexer somehow");
          case Token::BraceL:
            new_state = ParseLoopBegin();
            break;
          case Token::BraceR:
            new_state = ParseLoopEnd();
            break;
          case Token::Plus:
          case Token::Minus:
            new_state = ParseCellModify();
            break;
          case Token::Comma:
            new_state = ParseCellWrite();
            break;
          case Token::Period:
            new_state = ParseCellPrint();
            break;
          case Token::ChevronR:
          case Token::ChevronL:
            new_state = ParseShift();
            break;
          case Token::Eof:
            AtomicState state {
                .payload = 0,
                .tag = AtomicStateTag::Eof,
            };
            states.push_back(state);
            return states;
        }

        states.push_back(new_state);
    }

    driver_.ParseError("Expected EOF token somewhere lmao");
    __builtin_unreachable();
}

AtomicState AtomicStateParser::ParseLoopBegin() {
    driver_.Expect(Token::BraceL);
    AtomicState state {
        .payload = 0,
        .tag = AtomicStateTag::LoopBegin,
    };
    return state;
}

AtomicState AtomicStateParser::ParseLoopEnd() {
    driver_.Expect(Token::BraceR);
    AtomicState state {
        .payload = 0,
        .tag = AtomicStateTag::LoopEnd,
    };
    return state;
}

AtomicState AtomicStateParser::ParseCellModify() {
    CellValue incr_amount = 0;

    // accumulate consecutive '+'/'-' tokens into single atom
    for (;;) {
        if (driver_.Accept(Token::Plus)) {
            incr_amount++;
        } else if (driver_.Accept(Token::Minus)) {
            incr_amount--;
        } else { // out of '+'/'-' symbols
            goto done;
        }

        // Increasing/decreasing the count further could possibly cause an
        // overflow in the final struct; we pessimistically return if so.
        // Any following '+'/'-' tokens will form a separate AtomicState.
        if (incr_amount >= kMaxCellValue ||
            incr_amount <= kMinCellValue) {
            goto done;
        }
    }

done:
    AtomicState state {
        .payload = incr_amount,
        .tag = AtomicStateTag::CellModify,
    };
    return state;
}

AtomicState AtomicStateParser::ParseCellWrite() {
    driver_.Expect(Token::Comma);
    AtomicState state {
        .payload = 0,
        .tag = AtomicStateTag::CellWrite,
    };
    return state;
}

AtomicState AtomicStateParser::ParseCellPrint() {
    driver_.Expect(Token::Period);
    AtomicState state {
        .payload = 0,
        .tag = AtomicStateTag::CellPrint,
    };
    return state;
}

AtomicState AtomicStateParser::ParseShift() {
    ShiftDist shift_amount = 0;

    // accumulate consecutive '>'/'<' tokens into single atom
    for (;;) {
        if (driver_.Accept(Token::ChevronR)) {
            shift_amount++;
        } else if (driver_.Accept(Token::ChevronL)) {
            shift_amount--;
        } else {
            goto done;
        }

        // Increasing/decreasing the count further could possibly cause an
        // overflow in the final struct; we pessimistically return if so.
        // Any following '>'/'<' tokens will form a separate AtomicState.
        if (shift_amount >= kMaxShiftDist ||
            shift_amount <= kMinShiftDist) {
            goto done;
        }
    }

done:
    AtomicState state {
        .payload = shift_amount,
        .tag = AtomicStateTag::Shift,
    };
    return state;
}


std::vector<MachineState> MachineStateParser::ParseMachineStates() {
    std::vector<MachineState> states;

    for ( ; ; ) {
        return states;
    }
}

MachineState MachineStateParser::ParseBranchState() {
    MachineStateTag tag;

    if (driver_.Accept(AtomicStateTag::LoopBegin)) {
        tag = MachineStateTag::BranchForward;
    } else {
        driver_.Expect(AtomicStateTag::LoopEnd);
        tag = MachineStateTag::BranchBackward;
    }

    MachineState state {
        .payload = MachineStatePayload {
            .offset = BranchOffset {
                .offset = 0 // Must be handled by the linker
            }
        },
        .tag = tag,
    };

    return state;
}

MachineState MachineStateParser::ParseActionState() {
    // TODO
    //
    MachineState state {
        .payload = MachineStatePayload {
            .action = ActionState {
                .shift_distance = 0,
                .cell_addend = 0,
                .print = 0,
                .read = 0,
            }
        },
        .tag = MachineStateTag::Action,
    };

    if (driver_.Accept(AtomicStateTag::CellPrint)) {
        state.payload.action.print = 1;
    }

    while (driver_.Accept(AtomicStateTag::CellModify)) {
        state.payload.action.cell_addend += driver_.Accepted().payload;
    }

    if (driver_.Accept(AtomicStateTag::CellWrite)) {
        state.payload.action.read = 1;
        state.payload.action.cell_addend = 0;
    }

    while (driver_.Accept(AtomicStateTag::CellModify)) {
        state.payload.action.cell_addend += driver_.Accepted().payload;
    }

    while (driver_.Accept(AtomicStateTag::Shift)) {
        state.payload.action.shift_distance += driver_.Accepted().payload;
    }

    return state;
}
