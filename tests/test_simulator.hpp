#include <iostream>
#include <vector>

#include "simulator/turing.hpp"
#include "utils.hpp"

namespace test_simulator {

using turing::Move;
using turing::SimulatorLogger;
using turing::Transition;
using turing::TuringSimulator;

void test_simulator() {
    /*
    Example for calculate (x + 1) / 2

    #Q = {start, mr, div, div2, fin}
    #S = {0, 1}
    #G = {0, 1, B}
    #B = B
    #N = 1
    #q0 = start
    #F = {fin}

    start 0 0 r fin
    start 1 1 r mr
    start B B r fin

    mr 0 0 r mr
    mr 1 1 r mr
    mr B B l div

    div 0 B l fin
    div 1 0 l div2
    div B B r fin

    div2 0 1 r mr
    div2 1 0 l div2
    div2 B 1 r mr
    */
    char blank = '_';
    std::set<std::string> states = {"start", "mr", "div", "div2", "fin"};
    std::set<char> alphabet = {'0', '1'};
    std::set<char> tape_alphabet = {blank, '0', '1'};
    std::set<std::string> final_states = {"fin"};
    std::string start_state = "start";
    int num_tapes = 1;

    Transition<std::string, char> transition;
    transition.add_transition("start", {'0'}, "fin", {'0'}, {Move::FORWARD});
    transition.add_transition("start", {'1'}, "mr", {'1'}, {Move::FORWARD});
    transition.add_transition("start", {blank}, "fin", {blank},
                              {Move::FORWARD});

    transition.add_transition("mr", {'0'}, "mr", {'0'}, {Move::FORWARD});
    transition.add_transition("mr", {'1'}, "mr", {'1'}, {Move::FORWARD});
    transition.add_transition("mr", {blank}, "div", {blank}, {Move::BACKWARD});

    transition.add_transition("div", {'0'}, "fin", {blank}, {Move::BACKWARD});
    transition.add_transition("div", {'1'}, "div2", {'0'}, {Move::BACKWARD});
    transition.add_transition("div", {blank}, "fin", {blank}, {Move::FORWARD});

    transition.add_transition("div2", {'0'}, "mr", {'1'}, {Move::FORWARD});
    transition.add_transition("div2", {'1'}, "div2", {'0'}, {Move::BACKWARD});
    transition.add_transition("div2", {blank}, "mr", {'1'}, {Move::FORWARD});

    auto simulator = TuringSimulator<std::string, char>(
        states, alphabet, tape_alphabet, start_state, final_states, transition,
        blank, num_tapes);
    auto logger = SimulatorLogger<std::string, char>(false, std::cout);

    for (auto& input : {"10", "11", "100", "101", "110", "111", "fail"}) {
        try {
            auto result =
                simulator.run(utils::string_to_vector(input), &logger);
        } catch (TuringSimulatorException& e) {
            simulator.halt_by_error();
            logger.log_error(simulator, e);
        }
        simulator.reset();
    }
};

}  // namespace test_simulator
