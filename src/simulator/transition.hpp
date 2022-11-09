#ifndef __SIMULATOR_TRANSITION_HPP__
#define __SIMULATOR_TRANSITION_HPP__

#include <map>
#include <tuple>
#include <vector>

#include "tape.hpp"

namespace transition {

template <typename S, typename T>
using StateInput = std::tuple<S, std::vector<T>>;

template <typename S, typename T, typename M>
using StateOutputMove = std::tuple<S, std::vector<T>, std::vector<M>>;

using tape::Move;

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
}  // namespace transition

#endif  // __SIMULATOR_TRANSITION_HPP__