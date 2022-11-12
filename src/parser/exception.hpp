#ifndef __PARSER_EXCEPTION_HPP__
#define __PARSER_EXCEPTION_HPP__

#include <exception>
#include <string>

/**
 * Represents an exception that occurs during parsing of the turing machine
 * definition.
 */
class ParserException : public std::exception {
protected:
    const std::string msg_;
    const std::string context_;
    const size_t line_;
    const size_t column_;

public:
    ParserException(const std::string msg_)
        : msg_(msg_), context_(""), line_(0), column_(0) {}
    ParserException(const std::string msg_,
                    const std::string context,
                    size_t line,
                    size_t column)
        : msg_(msg_), context_(context), line_(line), column_(column) {}

    bool has_context() const { return context_.length() > 0; }
    const char* what() const noexcept { return msg_.c_str(); }
    const char* context() const noexcept { return context_.c_str(); }
    size_t line() const noexcept { return line_; }
    size_t column() const noexcept { return column_; }
};

#endif  // __PARSER_EXCEPTION_HPP__