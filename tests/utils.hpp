#ifndef __TEST_UTILS_HPP__
#define __TEST_UTILS_HPP__

#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

namespace utils {

std::vector<char> string_to_vector(const std::string& str) {
    return std::vector<char>(str.begin(), str.end());
}

std::string vector_to_string(const std::vector<char>& vec) {
    return std::string(vec.begin(), vec.end());
}

template <typename T>
std::string vector_to_string(const std::vector<T>& vec) {
    std::stringstream ss;
    for (const auto& item : vec) {
        ss << item << " ";
    }
    return ss.str();
}

template <typename T>
size_t get_width(const T& item) {
    std::stringstream ss;
    ss << item;
    return ss.str().size();
}

template <typename T>
std::string tape_to_string(const tape::Tape<T>& tape, bool verbose = true) {
    std::stringstream ss;
    if (!verbose) {
        for (int i = tape.front(); i < tape.back(); ++i) {
            ss << tape[i];
        }
    } else {
        int head = tape.head();
        int front = tape.front();
        int back = tape.back();

        int begin = std::min(front, head);
        int end = std::max(back, head + 1);

        std::stringstream index_ss;
        std::stringstream tape_ss;
        std::stringstream head_ss;
        // left-align each stream
        index_ss << std::left;
        tape_ss << std::left;
        head_ss << std::left;

        std::vector<int> indices(end - begin);
        std::iota(indices.begin(), indices.end(), begin);
        for_each(indices.begin(), indices.end(), [&](int i) {
            int index = (i < 0 ? -i : i);
            size_t width =
                std::max(std::to_string(index).length(), get_width<T>(tape[i]));
            index_ss << std::setw(width) << index << " ";
            tape_ss << std::setw(width) << tape[i] << " ";
            head_ss << std::setw(width) << (i == head ? "^" : " ") << " ";
        });
        ss << "INDEX: " << index_ss.str() << std::endl;
        ss << "TAPE:  " << tape_ss.str() << std::endl;
        ss << "HEAD:  " << head_ss.str() << std::endl;
    }
    return ss.str();
}

}  // namespace utils

#endif  // __TEST_UTILS_HPP__