#ifndef __PARSER_HPP__
#define __PARSER_HPP__

#include <fstream>
#include <iomanip>
#include <iostream>
#include <set>
#include <string>

#include "exception.hpp"
#include "reader.hpp"
#include "simulator/simulator.hpp"
#include "simulator/transition.hpp"

namespace strings {
const std::string whitespace = " \t\n\r\v\f";
const std::string digits = "0123456789";
const std::string hexdigits = digits + "abcdef" + "ABCDEF";
const std::string ascii_lowercase = "abcdefghijklmnopqrstuvwxyz";
const std::string ascii_uppercase = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const std::string ascii_letters = ascii_lowercase + ascii_uppercase;
const std::string punctuation = "!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~";
const std::string printable = digits + ascii_letters + punctuation + whitespace;

const std::string acceptable_states = ascii_letters + digits + "_";
const std::string acceptable_symbols =
    ascii_letters + digits +
    "!\"#$%&'()+-./:<=>?@[\\]^_`|~*";  // ,;{} \t\n\r\v\f removed, * special
const std::string acceptable_directions = "lr*";
const std::string definition = "QFSBGN";

const char COMMENT = ';';
const char COMMA = ',';
const char SHARP = '#';
const char EQUAL = '=';
const char LC = '{';
const char RC = '}';
const char WILD = '*';
}  // namespace strings

namespace parser {
class TuringParser {
protected:
    bool verbose_;

    const char* filename_;
    std::ifstream file_;
    reader::Reader reader_;

    bool parsed_;  // parse() can only be called once
    bool state_parsed_;
    bool input_symbol_parsed_;
    bool tape_symbol_parsed_;
    bool initial_state_parsed_;
    bool final_states_parsed_;
    bool transition_parsed_;
    bool blank_parsed_;
    bool n_tapes_parsed_;

    std::set<std::string> states;
    std::set<std::string> input_symbols;
    std::set<std::string> tape_symbols;
    std::set<char> input_symbols_char;
    std::set<char> tape_symbols_char;
    std::set<std::string> final_states;
    transition::WildcardTransition<std::string, char> transition;
    std::string initial_state;
    std::string blank;
    std::string n_tapes;
    size_t n_tapes_int;

    void remove_ws_and_comments() {
        while (true) {
            reader_.discard_while(strings::whitespace);
            if (reader_.peek() == strings::COMMENT) {
                reader_.discard_until("\r\n");
            } else {
                break;
            }
        }
    }

    void check_and_remove(char c) {
        check_unexpected_eof();
        char next = reader_.read_take();
        if (next != c) {
            throw ParserException("expected '" + std::string(1, c) + "'",
                                  reader_.take_line(), reader_.line(),
                                  reader_.column() - 1);
        }
    }

    void check_unexpected_eof() {
        if (reader_.peek() == EOF) {
            throw ParserException("unexpected EOF", reader_.take_line(),
                                  reader_.line(), reader_.column());
        }
    }
    void parse_definition() {
        // already ensured that the first character is #
        reader_.discard(1);
        check_unexpected_eof();
        // dispatch according to the definition
        const char lookahead = reader_.read_take();
        if (lookahead == 'q') {
            char zero = reader_.read_take();
            if (zero != '0') {
                std::string bad = zero == EOF ? "" : std::string(1, zero);
                throw ParserException("expected '#q0', got '#q" + bad + "'",
                                      reader_.take_line(), reader_.line(),
                                      reader_.column() - 3);
            }
        } else if (strings::definition.find(lookahead) == std::string::npos) {
            throw ParserException(
                "expected '#Q', '#S', '#G', '#q0', '#B', '#F' or '#N', got "
                "'#" +
                    std::string(1, lookahead) + "'",
                reader_.take_line(), reader_.line(), reader_.column() - 2);
        }
        // remove '='
        remove_ws_and_comments();
        check_and_remove(strings::EQUAL);
        switch (lookahead) {
            case 'Q':
                parse_set(states, state_parsed_, "'#Q'",
                          strings::acceptable_states);
                break;
            case 'F':
                parse_set(final_states, final_states_parsed_, "'#F'",
                          strings::acceptable_states);
                break;
            case 'S':
                parse_set(input_symbols, input_symbol_parsed_, "'#S'",
                          strings::acceptable_symbols, true);
                break;
            case 'G':
                parse_set(tape_symbols, tape_symbol_parsed_, "'#G'",
                          strings::acceptable_symbols, true);
                break;
            case 'B':
                parse_string(blank, blank_parsed_, "'#B'",
                             strings::acceptable_symbols, true);
                break;
            case 'N':
                parse_string(n_tapes, n_tapes_parsed_, "'#N'", strings::digits);
                n_tapes_int = static_cast<size_t>(std::stoi(n_tapes));
                break;
            case 'q':
                parse_string(initial_state, initial_state_parsed_, "'#q0'",
                             strings::acceptable_states);
                break;
            default:
                // should not reach here
                throw std::runtime_error("unreachable");
        }
    }

    void parse_string(std::string& s,
                      bool& parsed,
                      const std::string& name,
                      const std::string& acceptable,
                      bool is_single_char = false) {
        if (parsed) {
            throw ParserException("duplicate definition of " + name,
                                  reader_.take_line(), reader_.line(),
                                  reader_.column());
        }
        parsed = true;

        remove_ws_and_comments();
        reader_.read_while(acceptable);
        s = reader_.take();
        if (strings::whitespace.find(reader_.peek()) == std::string::npos &&
            reader_.peek() != EOF) {
            std::string bad = std::string(1, reader_.peek());
            throw ParserException("invalid character '" + bad + "' in " + name,
                                  reader_.take_line(), reader_.line(),
                                  reader_.column());
        }
        if (s.empty()) {
            throw ParserException("empty " + name, reader_.take_line(),
                                  reader_.line(), reader_.column());
        }
        if (is_single_char && s.size() != 1) {
            throw ParserException(
                "expected single character in " + name + ", got '" + s + "'",
                reader_.take_line(), reader_.line(),
                reader_.column() - s.size());
        }
    }

    void parse_set(std::set<std::string>& set,
                   bool& parsed,
                   const std::string& name,
                   const std::string& acceptable,
                   bool is_single_char = false) {
        if (parsed) {
            throw ParserException("duplicate definition of " + name,
                                  reader_.take_line(), reader_.line(),
                                  reader_.column() - 1);
        }
        parsed = true;

        // remove '{'
        remove_ws_and_comments();
        check_and_remove(strings::LC);

        const int start = 0, read_element = 1, finish_element = 2, stop = 3;
        bool followed_by_ws_or_comment = false;
        std::string parsing_element;
        int state = start;
        int last_cursor;
        while (state != stop) {
            switch (state) {
                case start:
                    remove_ws_and_comments();
                    check_unexpected_eof();
                    if (reader_.peek() == strings::RC) {
                        state = stop;
                        reader_.discard(1);
                    } else {
                        state = read_element;
                    }
                    break;
                case read_element:
                    remove_ws_and_comments();
                    check_unexpected_eof();
                    reader_.read_while(acceptable);
                    parsing_element = reader_.take();
                    check_unexpected_eof();
                    followed_by_ws_or_comment =
                        strings::whitespace.find(reader_.peek()) !=
                            std::string::npos ||
                        reader_.peek() == strings::COMMENT;
                    state = finish_element;
                    break;
                case finish_element:
                    if (parsing_element.empty()) {
                        throw ParserException("empty element in " + name,
                                              reader_.take_line(),
                                              reader_.line(), reader_.column());
                    }
                    if (is_single_char && parsing_element.size() != 1) {
                        throw ParserException(
                            "expected single character in " + name + ", got '" +
                                parsing_element + "'",
                            reader_.take_line(), reader_.line(),
                            reader_.column() - parsing_element.size());
                    }
                    if (set.find(parsing_element) != set.end()) {
                        throw ParserException(
                            "duplicate element '" + parsing_element + "' in " +
                                name,
                            reader_.take_line(), reader_.line(),
                            reader_.column() - parsing_element.size());
                    }
                    last_cursor = reader_.column();
                    if (followed_by_ws_or_comment) {
                        remove_ws_and_comments();
                        check_unexpected_eof();
                    }
                    switch (reader_.read_take()) {
                        case strings::RC:
                            state = stop;
                            break;
                        case strings::COMMA:
                            state = read_element;
                            break;
                        default:
                            std::string line = reader_.take_line();
                            if (followed_by_ws_or_comment) {
                                throw ParserException("expected ',' or '}'",
                                                      line, reader_.line(),
                                                      last_cursor);
                            } else {
                                // read_while stopped due to
                                // unacceptable character
                                std::string bad =
                                    std::string(1, reader_.peek());
                                throw ParserException("invalid character '" +
                                                          bad + "' in " + name,
                                                      line, reader_.line(),
                                                      reader_.column());
                            }
                    }
                    set.insert(parsing_element);
                    break;
                default:
                    // should not reach here
                    throw std::runtime_error("unreachable");
            }
        }
    }

    void check_transition_state(const std::string& state) {
        if (strings::whitespace.find(reader_.peek()) == std::string::npos &&
            reader_.peek() != strings::COMMENT && reader_.peek() != EOF) {
            // the following character is not ws, ';' or EOF
            std::string bad = std::string(1, reader_.peek());
            throw ParserException("invalid state character '" + bad + "'",
                                  reader_.take_line(), reader_.line(),
                                  reader_.column());
        }
        if (state.empty()) {
            throw ParserException("empty state", reader_.take_line(),
                                  reader_.line(), reader_.column());
        }
        if (states.find(state) == states.end()) {
            throw ParserException("state '" + state + "' not declared",
                                  reader_.take_line(), reader_.line(),
                                  reader_.column() - state.size());
        }
    }

    void check_transition_symbol(const std::vector<char>& symbols) {
        if (!n_tapes_parsed_) {
            throw ParserException("undefined number of tapes '#N'",
                                  reader_.take_line(), reader_.line(),
                                  reader_.column());
        }
        if (strings::whitespace.find(reader_.peek()) == std::string::npos &&
            reader_.peek() != strings::COMMENT) {
            // the following character is not ws or ';'
            std::string bad = std::string(1, reader_.peek());
            throw ParserException("invalid symbol character '" + bad + "'",
                                  reader_.take_line(), reader_.line(),
                                  reader_.column());
        }
        if (symbols.size() != n_tapes_int) {
            throw ParserException(
                "unmatched number of symbols with number of tapes, expected " +
                    std::to_string(n_tapes_int) + ", got " +
                    std::to_string(symbols.size()),
                reader_.take_line(), reader_.line(), reader_.column());
        }
        int i = 0;
        for (const char& symbol : symbols) {
            if (tape_symbols.find(std::string(1, symbol)) ==
                    tape_symbols.end() &&
                symbol != strings::WILD) {
                throw ParserException(
                    "symbol '" + std::string(1, symbol) + "' not declared",
                    reader_.take_line(), reader_.line(),
                    reader_.column() - symbols.size() + i);
            }
            i++;
        }
    }

    void parse_transition() {
        // already ensured that the first character is not ws, ';' or '#'
        // from ws input ws output ws direction ws state
        reader_.read_while(strings::acceptable_states);
        std::string from = reader_.take();
        check_transition_state(from);
        remove_ws_and_comments();
        check_unexpected_eof();

        reader_.read_while(strings::acceptable_symbols);
        std::string input_ = reader_.take();
        std::vector<char> input =
            std::vector<char>(input_.begin(), input_.end());
        check_transition_symbol(input);
        remove_ws_and_comments();
        check_unexpected_eof();

        if (transition.has_transition(from, input)) {
            throw ParserException("duplicate transition", reader_.take_line(),
                                  reader_.line(), reader_.column());
        }

        reader_.read_while(strings::acceptable_symbols);
        std::string output_ = reader_.take();
        std::vector<char> output =
            std::vector<char>(output_.begin(), output_.end());
        check_transition_symbol(output);
        remove_ws_and_comments();
        check_unexpected_eof();

        reader_.read_while(strings::acceptable_directions);
        std::string direction_ = reader_.take();
        std::vector<turing::Move> direction;
        std::transform(direction_.begin(), direction_.end(),
                       std::back_inserter(direction), [this](char c) {
                           switch (c) {
                               case 'l':
                                   return turing::Move::BACKWARD;
                               case 'r':
                                   return turing::Move::FORWARD;
                               case '*':
                                   return turing::Move::STAY;
                               default:
                                   throw std::runtime_error("unreachable");
                           }
                       });
        if (direction.size() != n_tapes_int) {
            throw ParserException(
                "unmatched number of directions with number of tapes, "
                "expected " +
                    std::to_string(n_tapes_int) + ", got " +
                    std::to_string(direction.size()),
                reader_.take_line(), reader_.line(), reader_.column());
        }
        remove_ws_and_comments();
        check_unexpected_eof();

        reader_.read_while(strings::acceptable_states);
        std::string to = reader_.take();
        check_transition_state(to);

        transition.add_transition(from, input, to, output, direction);
    }

    void check_integrity() {
        if (!state_parsed_) {
            throw ParserException("undefined state set '#Q'");
        }
        if (!input_symbol_parsed_) {
            throw ParserException("undefined input symbol set '#S'");
        }
        if (!tape_symbol_parsed_) {
            throw ParserException("undefined tape symbol set '#G'");
        }
        if (!initial_state_parsed_) {
            throw ParserException("undefined initial state '#q0'");
        }
        if (!final_states_parsed_) {
            throw ParserException("undefined final state set '#F'");
        }
        if (!n_tapes_parsed_) {
            throw ParserException("undefined number of tapes '#N'");
        }
        // check states in #Q
        for (auto state : final_states) {
            if (states.find(state) == states.end()) {
                throw ParserException("final state '" + state +
                                      "' not declared in '#Q'");
            }
        }
        if (states.find(initial_state) == states.end()) {
            throw ParserException("initial state '" + initial_state +
                                  "' not declared in '#Q'");
        }
        // check input symbols in tape symbols
        for (auto symbol : input_symbols_char) {
            if (tape_symbols_char.find(symbol) == tape_symbols_char.end()) {
                throw ParserException("input symbol '" +
                                      std::string(1, symbol) +
                                      "' not declared in '#G'");
            }
            if (symbol == strings::WILD) {
                throw ParserException("wildcard symbol '" +
                                      std::string(1, symbol) +
                                      "' not allowed in '#S'");
            }
        }
        if (tape_symbols.find(blank) == tape_symbols.end()) {
            throw ParserException("blank symbol '" + blank +
                                  "' not declared in '#G'");
        }
        if (tape_symbols_char.find(strings::WILD) != tape_symbols_char.end()) {
            throw ParserException("wildcard symbol '" +
                                  std::string(1, strings::WILD) +
                                  "' not allowed in '#G'");
        }
    }

public:
    TuringParser(bool verbose, const char* filename) try
        : verbose_(verbose),
          filename_(filename),
          file_(filename),
          reader_(file_),
          parsed_(false),
          state_parsed_(false),
          input_symbol_parsed_(false),
          tape_symbol_parsed_(false),
          initial_state_parsed_(false),
          final_states_parsed_(false),
          transition_parsed_(false),
          blank_parsed_(false),
          n_tapes_parsed_(false),
          transition(strings::WILD) {
        if (!file_.is_open()) {
            throw std::runtime_error("No such file or directory");
        }
    } catch (std::runtime_error const& e) {
        if (verbose_) {
            std::cerr << filename << ": " << e.what() << std::endl;
        } else {
            std::cerr << "cannot open file" << std::endl;
        }
        exit(1);
    }

    turing::TuringSimulator<std::string, char> parse() {
        try {
            if (parsed_) {
                throw std::runtime_error("parse() can only be called once");
            }
            parsed_ = true;
            while (!reader_.eof()) {
                remove_ws_and_comments();
                switch (reader_.peek()) {
                    case EOF:
                        break;
                    case strings::SHARP:  // definitions
                        parse_definition();
                        break;
                    default:  // transition
                        parse_transition();
                        break;
                }
            }

            // convert string symbols to char symbols
            std::transform(
                tape_symbols.begin(), tape_symbols.end(),
                std::inserter(tape_symbols_char, tape_symbols_char.end()),
                [](const std::string& s) { return s[0]; });
            std::transform(
                input_symbols.begin(), input_symbols.end(),
                std::inserter(input_symbols_char, input_symbols_char.end()),
                [](const std::string& s) { return s[0]; });
            // expand wildcards
            transition.initialize_transition(tape_symbols_char);
            // check integrity
            check_integrity();
            return turing::TuringSimulator<std::string, char>(
                states, input_symbols_char, tape_symbols_char, initial_state,
                final_states, std::move(transition), blank[0], n_tapes_int);
        } catch (std::runtime_error const& e) {
            if (verbose_) {
                std::cerr << filename_ << ": " << e.what() << std::endl;
            } else {
                std::cerr << "runtime error" << std::endl;
            }
            exit(1);
        } catch (ParserException const& e) {
            if (verbose_) {
                if (e.has_context()) {
                    std::cerr << filename_ << ":" << e.line() << ":"
                              << e.column() << ": " << e.what() << std::endl;
                    std::cerr << std::right << std::setw(5) << e.line() << " | "
                              << e.context() << std::endl;
                    std::cerr << std::right << std::setw(5) << " "
                              << " | " << std::string(e.column() - 1, ' ')
                              << "^" << std::endl;
                } else {
                    std::cerr << filename_ << ": " << e.what() << std::endl;
                }
            } else {
                std::cerr << "syntax error" << std::endl;
            }
            exit(1);
        }
    }
};

}  // namespace parser

#endif  // __PARSER_HPP__