#ifndef __EMULATOR_EXCEPTION_HPP__
#define __EMULATOR_EXCEPTION_HPP__

#include <cstring>
#include <exception>

/**
 * Represents an exception that occurs during the execution of the simulator.
 */
class TuringSimulatorException : public std::exception {
public:
    TuringSimulatorException(const char* msg, const char* verbose = nullptr)
        : msg_(msg), verbose_(verbose) {}
    const char* what() const noexcept { return msg_; }
    const char* verbose() const noexcept { return verbose_; }
    bool is(const char* msg) const noexcept {
        // this is ugly
        // better to use an enum instead of a string to represent the error type
        return strcmp(msg, msg_) == 0;
    }

private:
    const char* msg_;
    const char* verbose_;
};

#endif  // __EMULATOR_EXCEPTION_HPP__