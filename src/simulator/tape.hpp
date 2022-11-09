#ifndef __EMULATOR_TAPE_HPP__
#define __EMULATOR_TAPE_HPP__

#include <sstream>
#include <tuple>
#include <vector>

namespace tape {

typedef int pos_t;
enum class Move { FORWARD = 1, BACKWARD = -1, STAY = 0 };

template <typename T>
class Tape {
protected:
    T blank_;              // blank symbol in the tape
    std::vector<T> tape_;  // tape
    pos_t head_;           // read/write head position
    pos_t front_;          // first non-zero cell position

    void squeeze() {
        // remove leading and trailing blanks, but leave at least one cell
        while (tape_.size() > 1 && tape_.front() == blank_) {
            tape_.erase(tape_.begin());
            ++front_;
        }
        while (tape_.size() > 1 && tape_.back() == blank_) {
            tape_.pop_back();
        }
        if (empty()) {
            front_ = head_;
        }
    }

public:
    Tape(T blank, const std::vector<T>& tape = {})
        : blank_(blank), tape_(tape), head_(0), front_(0) {
        if (tape_.empty()) {
            tape_.push_back(blank_);
        }
    }

    pos_t head() const { return head_; }
    pos_t front() const { return front_; }
    pos_t back() const { return front_ + tape_.size(); }
    const std::vector<T>& tape() const { return tape_; }
    bool empty() const { return tape_.size() == 1 && tape_.front() == blank_; }
    bool is_blank(const T& symbol) const { return symbol == blank_; }
    void reset() {
        tape_.clear();
        tape_.push_back(blank_);
        head_ = 0;
        front_ = 0;
    }

    const T& operator[](pos_t index) const {
        // if index is out of range, return blank symbol
        // otherwise return the cell value
        return (index >= front() && index < back()) ? tape_[index - front_]
                                                    : blank_;
    }
    const T& read() const { return (*this)[head_]; }
    pos_t write(const T& symbol, Move move = Move::STAY) {
        // if the symbol is blank, remove the cell if head is inside the tape
        // if the symbol is not blank, expand the tape if necessary and write
        if (symbol != blank_ || (head_ >= front() && head_ < back())) {
            if (head_ < front_) {
                tape_.insert(tape_.begin(), front_ - head_, blank_);
                front_ = head_;
            } else if (head_ >= back()) {
                tape_.insert(tape_.end(), head_ - back() + 1, blank_);
            }
            tape_[head_ - front_] = symbol;
        }
        head_ += static_cast<pos_t>(move);
        squeeze();
        return head_;
    }
    pos_t erase(Move move = Move::STAY) { return write(blank_, move); }
};

template <typename T>
std::string to_string(const tape::Tape<T>& tape) {
    std::ostringstream ss;
    if (!tape.empty()) {
        for (pos_t i = tape.front(); i < tape.back(); ++i) {
            ss << tape[i];
        }
    }
    return ss.str();
}

template <typename T>
class MultiTape {
protected:
    std::vector<Tape<T>> tapes_;
    T blank_;

public:
    MultiTape(T blank, size_t n_tapes)
        : tapes_(n_tapes, Tape<T>(blank)), blank_(blank) {}

    auto begin() const { return tapes_.begin(); }
    auto end() const { return tapes_.end(); }
    auto begin() { return tapes_.begin(); }
    auto end() { return tapes_.end(); }
    auto cbegin() const { return tapes_.cbegin(); }
    auto cend() const { return tapes_.cend(); }
    size_t size() const { return tapes_.size(); }

    void reset() {
        for (auto& tape : tapes_) {
            tape.reset();
        }
    }
    const Tape<T>& operator[](size_t index) const { return tapes_[index]; }
    void set_tape(size_t index, const std::vector<T>& tape) {
        tapes_[index] = Tape<T>(blank_, tape);
    }
    std::vector<T> get_tape(size_t index) const {
        return tapes_[index].empty() ? std::vector<T>() : tapes_[index].tape();
    }

    std::vector<T> read() const {
        std::vector<T> symbols;
        std::for_each(tapes_.begin(), tapes_.end(),
                      [&symbols](const Tape<T>& tape) {
                          symbols.push_back(tape.read());
                      });
        return symbols;
    }
    std::vector<pos_t> write(
        const std::vector<std::tuple<T, Move>>& symbol_moves) {
        std::vector<pos_t> heads;
        std::transform(tapes_.begin(), tapes_.end(), symbol_moves.begin(),
                       std::back_inserter(heads),
                       [](Tape<T>& tape, const auto& symbol_move) {
                           const auto [symbol, move] = symbol_move;
                           return tape.write(symbol, move);
                       });
        return heads;
    }
    std::vector<pos_t> erase(const std::vector<Move>& moves) {
        std::vector<pos_t> heads;
        std::transform(
            tapes_.begin(), tapes_.end(), moves.begin(),
            std::back_inserter(heads),
            [](Tape<T>& tape, const Move& move) { return tape.erase(move); });
        return heads;
    }
};
}  // namespace tape

#endif  // __EMULATOR_TAPE_HPP__