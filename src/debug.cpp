#include "debug.h"

#include <iostream>
#include <algorithm>
#include <list>

void PrintStates(std::vector<AtomicState>& states) {
    std::setbuf(stdout, NULL);

    int atom_count = 0;
    auto PrintState = [&](const AtomicState& state) {
        std::cout<<"Atom "<<atom_count++<<": ";
        switch (state.tag) {
            case Eof:
                std::cout<<"Eof";
                break;
            case LoopBegin:
                std::cout<<"LoopBegin";
                break;
            case LoopEnd:
                std::cout<<"LoopEnd";
                break;
            case CellModify:
                std::cout<<"CellModify - "<<static_cast<int>(state.payload);
                break;
            case CellWrite:
                std::cout<<"CellWrite";
                break;
            case CellPrint:
                std::cout<<"CellPrint";
                break;
            case Shift:
                std::cout<<"Shift - "<<static_cast<int>(state.payload);
                break;
        }
        std::cout<<std::endl;
    };

    for_each(states.begin(), states.end(), PrintState);
}

void PrintTokens(std::vector<Token>& tokens) {
    std::setbuf(stdout, NULL);

    int token_count = 0;
    auto PrintToken = [&](const Token tok) {
        std::cout<<"Token "<<token_count++<<": ";
        switch (tok) {
            case Token::Eof:
                std::cout<<"EOF";
                break;
            case Token::Invalid:
                std::cout<<"Invalid";
                break;
            default:
                std::cout<<"'"<<tokenToChar(tok)<<"'";
                break;
        };
        std::cout<<std::endl;
    };

    for_each(tokens.begin(), tokens.end(), PrintToken);
}

void InterpretStates(std::vector<AtomicState>& states, std::istream& input) {
    using PrintT = char;

    std::list<char> tape; // We use a list to allow free expansion on either side
    tape.push_back(0);
    auto mp = states.begin();
    auto tp = tape.begin();

    // Each case in the switch handles moving the machine pointer independently
    bool step = false;
    while (mp != states.end()) {
        if (step) {
            int x;
            std::cin>>x;
            std::cout<<"[[ ";
            for (auto itr = tape.begin(); itr != tape.end(); itr++) {
                std::cout<<static_cast<int>(*itr)<<" ";
            }
            std::cout<<"]]"<<std::endl;
        }
        switch(mp->tag) {
            case LoopBegin:
                /* DEBUG */ step = true;
                mp++;
                if (*tp == 0) {
                    for (int diff = 1; diff > 0; mp++) {
                        if (mp == states.end()) {
                            std::cerr<<"Unmatched '[' found!"<<std::endl;
                            std::terminate();
                        } else if (mp->tag == AtomicStateTag::LoopBegin) {
                            diff++;
                        } else if (mp->tag == AtomicStateTag::LoopEnd) {
                            diff--;
                        }
                    }
                }
                break;
            case LoopEnd:
                if (*tp != 0) {
                    mp--;
                    for (int diff = -1; diff < 0; mp--) {
                        if (mp->tag == AtomicStateTag::LoopBegin) {
                            diff++;
                        } else if (mp->tag == AtomicStateTag::LoopEnd) {
                            diff--;
                        } else if (mp == states.begin()) {
                            std::cerr<<"Unmatched '[' found!"<<std::endl;
                            std::terminate();
                        }
                    }
                }
                mp++;
                break;
            case CellModify:
                // Add to current tape cell
                *tp += mp->payload;
                mp++;
                break;
            case CellWrite:
                // Naively read stdin to current tape cell
                input>>*tp;
                mp++;
                break;
            case CellPrint:
                std::cout<<static_cast<PrintT>(*tp);
                mp++;
                break;
            case Shift:
                if (mp->payload > 0) {
                    for (int i = mp->payload; i > 0; i--) {
                        tp++;
                        if (tp == tape.end()) {
                            tape.push_back (0);
                            tp = std::prev(tape.end());
                        } else {
                            tp--;
                        }
                    }
                } else if (mp->payload < 0) {
                    for (int i = mp->payload; i < 0; i++) {
                        if (tp == tape.begin()) {
                            tape.push_front (0);
                            tp = tape.begin();
                        } else {
                            tp--;
                        }
                    }
                }
                mp++;
                break;
            case Eof:
                return;
        }
    }
}
