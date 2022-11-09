#include <cstring>

#include "test_simulator.hpp"
#include "test_tape.hpp"

int main(int argc, char** argv) {
    if (argc == 2) {
        if (strcmp(argv[1], "test_simulator") == 0) {
            test_simulator::test_simulator();
        } else if (strcmp(argv[1], "test_tape") == 0) {
            test_tape::test_tape();
            test_tape::test_multi_tape();
        }
    }
    return 0;
}