#include <cstring>

#include "test_simulator.hpp"
#include "test_tape.hpp"
#include "test_transition.hpp"

int main(int argc, char** argv) {
    if (argc == 2) {
        if (strcmp(argv[1], "test_simulator") == 0) {
            test_simulator::test_simulator();
        } else if (strcmp(argv[1], "test_tape") == 0) {
            test_tape::test_tape();
            test_tape::test_multi_tape();
        } else if (strcmp(argv[1], "test_transition") == 0) {
            test_transition::test_wildcard_transition();
        }
    }
    return 0;
}