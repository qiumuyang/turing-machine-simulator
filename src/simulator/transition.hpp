#ifndef __SIMULATOR_TRANSITION_HPP__
#define __SIMULATOR_TRANSITION_HPP__

#include <algorithm>
#include <map>
#include <set>
#include <tuple>
#include <vector>

#include <iostream>
#include "tape.hpp"
namespace transition {

template <typename S, typename T>
using StateInput = std::tuple<S, std::vector<T>>;

template <typename S, typename T, typename M>
using StateOutputMove = std::tuple<S, std::vector<T>, std::vector<M>>;

template <typename S, typename T, typename M>
using TransitionTuple =
    std::tuple<S, std::vector<T>, S, std::vector<T>, std::vector<M>>;

using tape::Move;

template <typename T>
std::set<std::vector<T>> all_permutation(const std::set<T>& input, size_t k);

template <typename S, typename T>
class Transition {
protected:
    std::map<StateInput<S, T>, StateOutputMove<S, T, Move>> transition_;

public:
    bool has_transition(const S& state, const std::vector<T>& input) const {
        return transition_.find(std::make_tuple(state, input)) !=
               transition_.end();
    }
    StateOutputMove<S, T, Move> get_transition(
        const S& state,
        const std::vector<T>& input) const {
        return transition_.at(std::make_tuple(state, input));
    }
    void add_transition(const S& state,
                        const std::vector<T>& input,
                        const S& next_state,
                        const std::vector<T>& output,
                        const std::vector<Move>& move) {
        transition_[std::make_tuple(state, input)] =
            std::make_tuple(next_state, output, move);
    }
};

template <typename S, typename T>
class WildcardTransition : public Transition<S, T> {
protected:
    using Transition<S, T>::transition_;
    T wildcard_;

public:
    using Transition<S, T>::has_transition;
    using Transition<S, T>::get_transition;
    using Transition<S, T>::add_transition;

    WildcardTransition(const T& wildcard) : wildcard_(wildcard) {}

    /**
     * Convert transitions containing wildcards to normal transitions.
     *
     * Rule: <state_from> <tape_read *> <tape_write *> <tape_move> <state_to>
     *       - wildcard in tape_read will be expanded to all possible alphabet
     *         if not defined explicitly
     *       - wildcard in tape_write should be in the same position as the
     *         wildcard in tape_read and will be replaced by corresponding read
     */
    void initialize_transition(const std::set<T>& alphabet) {
        std::map<S, std::set<std::vector<T>>> state_explicit_input;
        std::map<S, std::vector<TransitionTuple<S, T, Move>>>
            state_wildcard_transition;
        size_t input_length = 0;
        // 1. put transitions into different categories
        std::vector<StateInput<S, T>> to_be_removed;
        for (auto& [state_input, state_output_move] : transition_) {
            auto [state, input] = state_input;
            auto [next_state, output, move] = state_output_move;
            input_length = input.size();
            if (std::find(input.begin(), input.end(), wildcard_) !=
                input.end()) {
                // wildcard transition
                state_wildcard_transition[state].push_back(
                    std::make_tuple(state, input, next_state, output, move));
                // erase from transition_
                to_be_removed.push_back(state_input);
            } else {
                // explicit input
                state_explicit_input[state].insert(input);
            }
        }
        for (auto& state_input : to_be_removed) {
            transition_.erase(state_input);
        }
        // 2. check wildcard transition for each state
        for (auto& [state, wildcard_transition] : state_wildcard_transition) {
            // 2.1 sort wildcard transition by the number of wildcards
            std::sort(wildcard_transition.begin(), wildcard_transition.end(),
                      [this](const TransitionTuple<S, T, Move>& a,
                             const TransitionTuple<S, T, Move>& b) {
                          return std::count(std::get<1>(a).begin(),
                                            std::get<1>(a).end(), wildcard_) <
                                 std::count(std::get<1>(b).begin(),
                                            std::get<1>(b).end(), wildcard_);
                      });
            std::set<std::vector<T>> possible_input =
                all_permutation(alphabet, input_length);
            possible_input.erase(state_explicit_input[state].begin(),
                                 state_explicit_input[state].end());
            // 2.2 expand wildcard, update transition_ & state_explicit_input
            for (auto& [state, input, next_state, output, move] :
                 wildcard_transition) {
                std::set<std::vector<T>> expanded_input;
                for (const auto& possible : possible_input) {
                    // check possible matches input
                    auto new_input = input;
                    auto new_output = output;
                    bool match = true;
                    for (size_t i = 0; i < input.size(); ++i) {
                        if (input[i] != possible[i] && input[i] != wildcard_) {
                            match = false;
                            break;
                        }
                        if (input[i] == wildcard_) {
                            new_input[i] = possible[i];
                            if (output[i] == wildcard_) {
                                new_output[i] = possible[i];
                            }
                            // else: output[i] is not wildcard, keep it
                        } else if (output[i] == wildcard_) {
                            // input[i] is not wildcard
                            // but output[i] is wildcard
                            // throw error
                            throw std::runtime_error(
                                "wildcard in write symbols should be in the "
                                "same position as the wildcard in read "
                                "symbols");
                        }
                    }
                    if (match && state_explicit_input[state].find(new_input) ==
                                     state_explicit_input[state].end()) {
                        add_transition(state, new_input, next_state, new_output,
                                       move);
                        // state_explicit_input[state].insert(
                        //     std::vector<T>(new_input.begin(), new_input.end()));
                        expanded_input.insert(possible);
                    }
                }

                // possible_input.erase(expanded_input.begin(),
                //                      expanded_input.end());
                // std::cout << "erase done" << std::endl;
            }
        }
        std::cout << "initialize_transition done" << std::endl;
    }
};

template <typename T>
std::set<std::vector<T>> all_permutation(const std::set<T>& input, size_t n) {
    // sort at first and then use std::next_permutation
    std::set<std::vector<T>> result;
    std::vector<T> input_sorted = std::vector<T>(input.begin(), input.end());
    std::sort(input_sorted.begin(), input_sorted.end());
    do {
        result.insert(
            std::vector<T>(input_sorted.begin(), input_sorted.begin() + n));
    } while (std::next_permutation(input_sorted.begin(), input_sorted.end()));
    return result;
}
}  // namespace transition

#endif  // __SIMULATOR_TRANSITION_HPP__