#ifndef __EMULATOR_LOGGER_HPP__
#define __EMULATOR_LOGGER_HPP__

#include <sstream>
#include <vector>

// external class turing::TuringSimulator
// declare here due to circular dependency
namespace turing {
template <typename S, typename T>
class TuringSimulator;
}

namespace logger {
template <typename S, typename T>
class SimulatorLogger {
protected:
    bool verbose_;
    std::vector<std::ostream*> streams_;
    std::stringstream buffer_;

    void flush() {
        std::string str = buffer_.str();
        for (auto stream : streams_) {
            *stream << str;
        }
        buffer_.str(std::string());
    }

public:
    SimulatorLogger() : verbose_(false) {}
    SimulatorLogger(std::ostream& stream)
        : verbose_(false), streams_({&stream}) {}
    SimulatorLogger(bool verbose, std::ostream& stream)
        : verbose_(verbose), streams_({&stream}) {}
    SimulatorLogger(bool verbose, std::vector<std::ostream*> streams)
        : verbose_(verbose), streams_(streams) {}

    void log_step(const turing::TuringSimulator<S, T>& simulator) {
        // TODO: implement
        if (!verbose_) {
            return;
        }
        // dummy for unused variable warning
        buffer_ << "step: " << simulator.steps() << std::endl;
        flush();
    }
};
}  // namespace logger

#endif  // __EMULATOR_LOGGER_HPP__