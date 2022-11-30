#include <algorithm>
#include <iostream>

#include "simulator/transition.hpp"
#include "utils.hpp"

namespace test_transition {

using tape::Move;
using transition::WildcardTransition;

void test_wildcard_transition() {
    auto transition_ = WildcardTransition<std::string, char>('*');
    transition_.add_transition("q0", {'b', 'a'}, "q1", {'_', '_'},
                               {Move::STAY, Move::STAY});
    transition_.add_transition("q0", {'a', '*'}, "q2", {'_', '_'},
                               {Move::STAY, Move::STAY});
    transition_.initialize_transition({'a', 'b', '_'});
    for (const auto& [state_input, state_output_move] : transition_) {
        const auto& [state, input] = state_input;
        const auto& [next_state, output, move] = state_output_move;
        std::cout << state << " " << utils::vector_to_string(input) << " "
                  << next_state << " " << utils::vector_to_string(output) << " "
                  << std::endl;
    }
}

}  // namespace test_transition