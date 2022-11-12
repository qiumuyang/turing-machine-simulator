#ifndef __PARSER_READER_HPP__
#define __PARSER_READER_HPP__

#include <cassert>
#include <set>
#include <sstream>
#include <string>

namespace reader {

class Reader {
protected:
    size_t line_;
    size_t column_;
    std::stringstream buffer_;
    std::stringstream line_buffer_;
    std::istream& stream_;

public:
    Reader(std::istream& stream) : line_(1), column_(1), stream_(stream) {}

    size_t line() const { return line_; }
    size_t column() const { return column_; }
    bool eof() const { return stream_.eof(); }

    char peek() { return stream_.peek(); }
    /**
     * Take string from the buffer and clear it.
     */
    std::string take() {
        std::string str = buffer_.str();
        buffer_.str("");
        return str;
    }
    /**
     * Take the current whole line from the line buffer.
     *
     * @note ONLY call this on exception.
     */
    std::string take_line() {
        read_until("\r\n");
        std::string str = line_buffer_.str();
        // strip str beginning with \r or \n
        if (str.length() > 0 && (str[0] == '\r' || str[0] == '\n')) {
            str = str.substr(1);
        }
        line_buffer_.str("");
        return str;
    }
    /**
     * Read n characters from the stream to the buffer.
     */
    void read(int n = 1) {
        char c;
        for (int i = 0; i < n && stream_ >> std::noskipws >> c; ++i) {
            if (c == '\n') {
                ++line_;
                column_ = 1;
                line_buffer_.str("");
            } else {
                ++column_;
            }
            if (c == '\r' || c == '\0') {
                --column_;
                continue;
            }
            line_buffer_ << c;
            buffer_ << c;
        }
    }
    void discard(int n = 1) {
        for (int i = 0; i < n && !stream_.eof(); ++i) {
            char c;
            stream_ >> std::noskipws >> c;
            if (c == '\n') {
                ++line_;
                column_ = 1;
                line_buffer_.str("");
            } else {
                ++column_;
            }
            if (c == '\r' || c == '\0') {
                --column_;
                continue;
            }
            line_buffer_ << c;
        }
    }
    void read_until(const std::string& str) {
        while (!stream_.eof() &&
               str.find(stream_.peek()) == std::string::npos) {
            read();
        }
    }
    void read_while(const std::string& str) {
        while (!stream_.eof() &&
               str.find(stream_.peek()) != std::string::npos) {
            read();
        }
    }
    void discard_until(const std::string& str) {
        while (!stream_.eof() &&
               str.find(stream_.peek()) == std::string::npos) {
            discard();
        }
    }
    void discard_while(const std::string& str) {
        while (!stream_.eof() &&
               str.find(stream_.peek()) != std::string::npos) {
            discard();
        }
    }
    /**
     * Read and take a single character from the stream.
     *
     * @note ONLY call this when the buffer is empty.
     */
    char read_take() {
        assert(buffer_.str().empty());
        if (stream_.eof()) {
            return EOF;
        }
        read();
        std::string str = take();
        return str.empty() ? EOF : str[0];
    }
};

}  // namespace reader

#endif  // __PARSER_READER_HPP__