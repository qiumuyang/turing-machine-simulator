#ifndef __TURING_HPP__
#define __TURING_HPP__

#include <set>

#include "exception.hpp"
#include "logger.hpp"
#include "tape.hpp"
#include "transition.hpp"

namespace turing {
using logger::SimulatorLogger;
using tape::Move;
using tape::MultiTape;
using transition::StateInput;
using transition::StateOutputMove;
using transition::Transition;

/**
 * Emulates a Turing machine.
 *
 * @tparam S the type of states
 * @tparam T the type of symbols (alphabet for both input and tape)
 *
 * @note The definition of the Turing machine is not verified by the simulator
 * (though most of them will not do harm). Following additional checks are
 * expected to be performed by the user:
 * - initial state / final states are in the set of states
 * - blank symbol is in the alphabet
 * - states and symbols in transitions are in the set of states and alphabet
 * respectively
 * - the number of symbols in each transition matches the number of tapes
 *
 * @throw TuringSimulatorException (runtime) when:
 * - the input contains a symbol not in the alphabet
 * - step() is called when the simulator is halted
 * - set_input() is called when the simulator has started
 * - get_output() is called when the simulator has not halted
 */
template <typename S, typename T>
class TuringSimulator {
protected:
    // definition
    std::set<S> states_;           // set of states
    std::set<T> alphabet_;         // input alphabet
    std::set<T> tape_alphabet_;    // tape alphabet
    S initial_state_;              // initial state
    std::set<S> final_states_;     // set of final states
    Transition<S, T> transition_;  // transition function
    T blank_;                      // blank symbol

    // execution
    S state_;      // current state
    int step_;     // current step
    bool halted_;  // whether the simulator has halted

    // definition & execution
    MultiTape<T> tape_;  // tape

public:
    TuringSimulator(const std::set<S>& states,
                    const std::set<T>& alphabet,
                    const std::set<T>& tape_alphabet,
                    const S& initial_state,
                    const std::set<S>& final_states,
                    const Transition<S, T>& transition,
                    const T& blank,
                    size_t tape_num)
        : states_(states),
          alphabet_(alphabet),
          tape_alphabet_(tape_alphabet),
          initial_state_(initial_state),
          final_states_(final_states),
          transition_(transition),
          blank_(blank),
          state_(initial_state),
          step_(0),
          halted_(false),
          tape_(blank, tape_num) {}

    const S& state() const { return state_; }
    int steps() const { return step_; }

    bool started() const { return step_ > 0; }
    bool halted() const { return halted_; }
    void reset() {
        state_ = initial_state_;
        step_ = 0;
        halted_ = false;
        tape_.reset();
    }
    void set_input(const std::vector<T>& input) {
        if (started()) {
            throw TuringSimulatorException(
                "illegal operation",
                "cannot set input after the simulator has started");
        }
        // check input symbol is in the alphabet
        for (const auto& symbol : input) {
            if (alphabet_.find(symbol) == alphabet_.end()) {
                // TODO: verbose
                throw TuringSimulatorException(
                    "illegal input", "input symbol is not in the alphabet");
            }
        }
        tape_.set_tape(0, input);
    }
    std::vector<T> get_output() const {
        if (!halted()) {
            throw TuringSimulatorException(
                "illegal operation",
                "cannot get output before the simulator has halted");
        }
        return tape_.get_tape(0);
    }
    /**
     * Executes one step of the simulator.
     *
     * @param logger the logger to log the execution
     * @return false if the simulator has halted, true otherwise
     */
    bool step(SimulatorLogger<S, T>* logger = nullptr) {
        if (halted()) {
            throw TuringSimulatorException(
                "illegal operation",
                "cannot step after the simulator has halted");
        }
        // read tape
        auto input = tape_.read();
        // find transition
        auto state_input = std::make_tuple(state_, input);
        if (!transition_.has_transition(state_, input)) {
            // no transition found
            halted_ = true;
            return false;
        }
        auto [next_state, output, move] =
            transition_.get_transition(state_, input);
        auto symbol_move = std::vector<std::tuple<T, Move>>();
        for (size_t i = 0; i < output.size(); ++i) {
            symbol_move.push_back(std::make_tuple(output[i], move[i]));
        }
        // write tape
        tape_.write(symbol_move);
        // change state
        state_ = next_state;
        step_++;
        // check halt
        if (final_states_.find(state_) != final_states_.end()) {
            halted_ = true;
        }
        // log
        if (logger != nullptr) {
            logger->log_step(*this);
        }
        return !halted_;
    }
    /**
     * Executes the simulator with the given input until it halts.
     *
     * @param input the input to the simulator
     * @param logger the logger to log the execution
     * @return the output of the simulator
     *
     * @note For multiple runs, reset() to reset running state before next run.
     */
    std::vector<T> run(const std::vector<T>& input,
                       SimulatorLogger<S, T>* logger = nullptr) {
        set_input(input);
        while (step(logger)) {
            // do nothing
        }
        return get_output();
    }
};
}  // namespace turing

#endif  // __TURING_HPP__