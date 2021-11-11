#ifndef SYLVANITE_INCLUDE_PARSER_H_
#define SYLVANITE_INCLUDE_PARSER_H_

#include <iostream>
#include <vector>
#include <string>
#include <type_traits>

#include "token.h"
#include "ast.h"
#include "lexer.h"

template<typename InputT, typename TagT, typename OutputT>
class ParserDriver {
    public:
        using TagFunction = const TagT&(const InputT&);

        ParserDriver(std::vector<InputT> values, TagFunction* fn) :
            input_stream_(values),
            position_(0),
            tagger_(fn),
            accepted_(dummy_) {}

        void ParseError [[noreturn]] (std::string msg) const {
            std::cerr << "Parse error at position " << position_ << ": "
                      << msg << std::endl;
            std::terminate();
        }

        // Allows the parser to retrieve tokens w/ value payloads
        // after they've been stepped over
        const InputT& Accepted() const {
            return accepted_;
        }

        const InputT& Peek() const {
            return input_stream_[position_];
        }

        bool Peek(TagT expected) const {
            return tagger_(input_stream_[position_]) == expected;
        }

        template<typename Q = InputT>
        typename std::enable_if<!std::is_same<Q, TagT>::value, bool>::type
        Peek(InputT expected) const {
            return Peek(tagger_(expected));
        }

        bool Accept(TagT expected) {
            if (tagger_(Peek()) == expected) {
                accepted_ = input_stream_[position_];
                position_++;
                return true;
            }
            return false;
        }

        template<typename Q = InputT>
        typename std::enable_if<!std::is_same<Q, TagT>::value, bool>::type
        Accept(InputT expected) {
            return Accept(tagger_(expected));
        }

        void Expect(TagT expected) {
            if (!Accept(expected)) {
                ParseError("expected token not found");
            }
        }

        template<typename Q = InputT>
        typename std::enable_if<!std::is_same<Q, TagT>::value, bool>::type
        Expect(InputT expected) {
            Expect(tagger_(expected));
        }

    private:
        std::vector<InputT> input_stream_;
        size_t position_;
        TagFunction* tagger_;
        InputT dummy_; // this is awful TODO
        InputT& accepted_;
};


class AtomicStateParser {
    public:
        AtomicStateParser(Lexer lexer) :
      //    lexer_(lexer),
            driver_(DriveLexer(lexer),
                    [](const Token& s) ->const Token& {
                        return s;
                    }) {}

        std::vector<AtomicState> ParseAtomicStates();
    private:
      //const Lexer& lexer_;
        ParserDriver<Token, Token, AtomicState> driver_;

        std::vector<Token> DriveLexer(Lexer& lexer);

        AtomicState ParseLoopBegin();
        AtomicState ParseLoopEnd();
        AtomicState ParseCellModify();
        AtomicState ParseCellWrite();
        AtomicState ParseCellPrint();
        AtomicState ParseShift();
};

class MachineStateParser {
    public:
        MachineStateParser(AtomicStateParser in_parser) :
      //    input_parser_(in_parser),
            driver_(DriveInputParser(in_parser),
                    [](const AtomicState& s) ->const AtomicStateTag& {
                        return s.tag;
                    }) {}

        std::vector<MachineState> ParseMachineStates();
    private:
      //const AtomicStateParser& input_parser_;
        ParserDriver<AtomicState, AtomicStateTag, MachineState> driver_;

        std::vector<AtomicState> DriveInputParser(AtomicStateParser& input_parser);

        MachineState ParseBranchState();
        MachineState ParseActionState();
};

#endif // SYLVANITE_INCLUDE_PARSER_H_
