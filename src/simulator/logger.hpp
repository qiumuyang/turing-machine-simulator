#ifndef __EMULATOR_LOGGER_HPP__
#define __EMULATOR_LOGGER_HPP__

#include <ostream>
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

public:
    SimulatorLogger() : verbose_(false) {}
    SimulatorLogger(std::ostream& stream)
        : verbose_(false), streams_({&stream}) {}
    SimulatorLogger(bool verbose, std::ostream& stream)
        : verbose_(verbose), streams_({&stream}) {}
    SimulatorLogger(bool verbose, std::vector<std::ostream*> streams)
        : verbose_(verbose), streams_(streams) {}

    void log(const turing::TuringSimulator<S, T>& simulator) const {
        // TODO: implement
        if (!verbose_) {
            return;
        }
        for (auto stream : streams_) {
            // dummy for unused variable warning
            *stream << "step: " << simulator.steps() << std::endl;
        }
    }
};
}  // namespace logger

#endif  // __EMULATOR_LOGGER_HPP__