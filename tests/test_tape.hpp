#include <algorithm>
#include <iostream>

#include "simulator/tape.hpp"
#include "utils.hpp"

namespace test_tape {

using tape::Move;

void test_tape() {
    auto tape = tape::Tape<std::string>("[BLANK]");
    std::cout << utils::tape_to_string(tape) << std::endl;
    tape.write("a", Move::FORWARD);
    tape.write("b", Move::FORWARD);
    tape.write("c", Move::FORWARD);
    std::cout << utils::tape_to_string(tape) << std::endl;
    tape.erase(Move::BACKWARD);
    std::cout << utils::tape_to_string(tape) << std::endl;
    tape.erase(Move::BACKWARD);
    std::cout << utils::tape_to_string(tape) << std::endl;
}

void test_multi_tape() {
    auto multi_tape = tape::MultiTape<char>('_', 3);
    // write tape with tuple<char, move>
    multi_tape.write({
        {'a', Move::FORWARD},
        {'b', Move::FORWARD},
        {'c', Move::FORWARD},
    });
    multi_tape.erase({Move::FORWARD, Move::STAY, Move::BACKWARD});
    std::for_each(multi_tape.begin(), multi_tape.end(), [](const auto& tape) {
        std::cout << utils::tape_to_string(tape) << std::endl;
    });
}

}  // namespace test_tape