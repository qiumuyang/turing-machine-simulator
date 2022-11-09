#ifndef __EMULATOR_LOGGER_HPP__
#define __EMULATOR_LOGGER_HPP__

#include <functional>
#include <iomanip>
#include <numeric>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

#include "exception.hpp"

// external class turing::TuringSimulator
// declare here due to circular dependency
namespace turing {
template <typename S, typename T>
class TuringSimulator;
}

namespace logger {
template <typename S, typename T>
using ExceptionLogger = std::function<void(const turing::TuringSimulator<S, T>&,
                                           const TuringSimulatorException&,
                                           std::ostream&)>;

template <typename S, typename T>
auto log_illegal_input =
    ExceptionLogger<S, T>([](const turing::TuringSimulator<S, T>& simulator,
                             const TuringSimulatorException& exception,
                             std::ostream& os) {
        (void)exception;  // unused
        // trick: when input is illegal, tape is not modified
        // so get_output() will return the first tape which is the input
        std::vector<T> input = simulator.get_output();
        auto alphabet = simulator.input_alphabet();

        for (auto [i, symbol] = std::tuple{0, input.begin()};
             symbol != input.end(); ++i, ++symbol) {
            if (alphabet.find(*symbol) == alphabet.end()) {
                os << "Error: "
                   << "'" << *symbol << "'"
                   << " was not declared in the set of input symbols"
                   << std::endl;
                os << "Input: ";
                std::for_each(input.begin(), input.end(),
                              [&](const T& symbol) { os << symbol; });
                os << std::endl;
                os << std::string(7 + i, ' ') << "^" << std::endl;
                break;
            }
        }
    });

template <typename S, typename T>
auto log_default_exception =
    ExceptionLogger<S, T>([](const turing::TuringSimulator<S, T>& simulator,
                             const TuringSimulatorException& exception,
                             std::ostream& os) {
        (void)simulator;  // unused
        os << "Error: "
           << (exception.verbose() == nullptr ? exception.what()
                                              : exception.verbose())
           << std::endl;
    });

template <typename T>
size_t get_width(const T& value) {
    std::stringstream ss;
    ss << value;
    return ss.str().size();
}

template <typename S, typename T>
class SimulatorLogger {
protected:
    std::string status_half_sep_ = std::string(21, '=');
    // status: half_sep + sp + 3 letters + sp + half_sep
    std::string step_sep_ = std::string(5 + 2 * 21, '-');
    std::string index_header_ = "Index";
    std::string tape_header_ = "Tape";
    std::string head_header_ = "Head";
    std::string state_header_ = "State";
    std::string step_header_ = "Step";
    std::string input_header_ = "Input";
    std::string result_header_ = "Result";
    std::string header_symbol_ = "^";
    std::string run_ = "RUN";
    std::string end_ = "END";
    std::string err_ = "ERR";

    bool verbose_;
    std::vector<std::ostream*> streams_;
    std::stringstream buffer_;

    void log_tape(int header_width, int index, const tape::Tape<T>& tape) {
        std::stringstream index_ss, tape_ss, head_ss;
        index_ss << std::left << std::setw(header_width)
                 << index_header_ + std::to_string(index) << " : ";
        tape_ss << std::left << std::setw(header_width)
                << tape_header_ + std::to_string(index) << " : ";
        head_ss << std::left << std::setw(header_width)
                << head_header_ + std::to_string(index) << " : ";

        int range_low = std::min(tape.front(), tape.head());
        int range_high = std::max(tape.back(), tape.head() + 1);
        std::vector<int> symbol_indices(range_high - range_low);
        std::iota(symbol_indices.begin(), symbol_indices.end(), range_low);

        std::for_each(symbol_indices.begin(), symbol_indices.end(), [&](int i) {
            auto t_idx = (i < 0 ? -i : i);
            auto header = (i == tape.head() ? header_symbol_ : " ");
            auto symbol = tape[i];
            size_t width = std::max(
                {get_width(t_idx), get_width(header), get_width(symbol)});
            index_ss << std::setw(width) << t_idx << " ";
            tape_ss << std::setw(width) << symbol << " ";
            head_ss << std::setw(width) << header << " ";
        });

        buffer_ << index_ss.str() << std::endl;
        buffer_ << tape_ss.str() << std::endl;
        buffer_ << head_ss.str() << std::endl;
    }

    void log_status(const std::string& status) {
        buffer_ << status_half_sep_ << " " << status << " " << status_half_sep_
                << std::endl;
    }

    void log_symbols(const std::vector<T>& values, const std::string sep = "") {
        std::for_each(values.begin(), values.end(),
                      [&](const T& value) { buffer_ << value << sep; });
    }

public:
    SimulatorLogger() : verbose_(false) {}
    SimulatorLogger(std::ostream& stream)
        : verbose_(false), streams_({&stream}) {}
    SimulatorLogger(bool verbose, std::ostream& stream)
        : verbose_(verbose), streams_({&stream}) {}
    SimulatorLogger(bool verbose, std::vector<std::ostream*> streams)
        : verbose_(verbose), streams_(streams) {}

    void flush(bool error = false) {
        std::string str = buffer_.str();
        if (error) {
            // only output to stderr if error
            std::cerr << str;
        } else {
            // output to all streams
            for (auto stream : streams_) {
                *stream << str;
            }
        }
        buffer_.str(std::string());
    }

    /**
     * @brief Log instantaneous description of the turing machine
     */
    void log_step(const turing::TuringSimulator<S, T>& simulator) {
        // TODO: implement
        if (!verbose_) {
            return;
        }
        auto current_step = simulator.steps();
        auto current_state = simulator.state();
        auto tape = simulator.tape();
        auto header_width = std::max({index_header_.size(), tape_header_.size(),
                                      head_header_.size(), state_header_.size(),
                                      step_header_.size()}) +
                            get_width(tape.size());
        // log current step
        buffer_ << std::left << std::setw(header_width) << step_header_ << " : "
                << current_step << std::endl;
        // log tapes
        for (auto [i, t] = std::tuple{0, tape.begin()}; t != tape.end();
             ++i, ++t) {
            log_tape(header_width, i, *t);
        }
        // log current state
        buffer_ << std::setw(header_width) << state_header_ << " : "
                << current_state << std::endl;
        // log separator
        buffer_ << step_sep_ << std::endl;
    }

    void log_input(const std::vector<T>& input) {
        if (!verbose_) {
            return;
        }
        // log input
        buffer_ << input_header_ << ": ";
        log_symbols(input);
        buffer_ << std::endl;
    }

    /**
     * @brief Log start of the turing machine
     */
    void log_start(const turing::TuringSimulator<S, T>& simulator) {
        if (!verbose_) {
            return;
        }
        // log run status
        log_status(run_);
        // log step 0
        log_step(simulator);
    }

    /**
     * @brief Log end of the turing machine and flush
     */
    void log_end(const turing::TuringSimulator<S, T>& simulator) {
        if (verbose_) {
            buffer_ << result_header_ << ": ";
        }
        log_symbols(simulator.get_output());
        buffer_ << std::endl;
        if (verbose_) {
            log_status(end_);
        }
        flush();
    }

    /**
     * @brief Log error of the turing machine
     */
    void log_error(const turing::TuringSimulator<S, T>& simulator,
                   const TuringSimulatorException& e) {
        if (!verbose_) {
            // simply log error
            buffer_ << e.what() << std::endl;
            flush(true);
            return;
        }
        // log err status
        log_status(err_);
        // log verbose error
        ExceptionLogger<S, T> exception_logger = log_default_exception<S, T>;
        if (e.is("illegal input")) {
            exception_logger = log_illegal_input<S, T>;
        }
        exception_logger(simulator, e, buffer_);
        // log end status
        log_status(end_);
        flush(true);
    }
};
}  // namespace logger

#endif  // __EMULATOR_LOGGER_HPP__