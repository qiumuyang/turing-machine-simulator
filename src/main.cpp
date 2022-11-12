#include "parser/parser.hpp"
#include "simulator/logger.hpp"
#include "simulator/simulator.hpp"

using namespace std;
using logger::SimulatorLogger;
using parser::TuringParser;
using turing::TuringSimulator;

int main(int argc, const char** argv) {
    const char* usage = "usage: turing [-v|--verbose] [-h|--help] <tm> <input>";

    if (argc < 3) {
        cout << usage << endl;
        return 1;
    }

    bool verbose = false;
    string tm_path;
    string input;

    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg == "-v" || arg == "--verbose") {
            verbose = true;
        } else if (arg == "-h" || arg == "--help") {
            cout << usage << endl;
            return 0;
        } else if (tm_path.empty()) {
            tm_path = arg;
        } else if (input.empty()) {
            input = arg;
        } else {
            cout << usage << endl;
            return 1;
        }
    }

    auto parser = TuringParser(verbose, tm_path.c_str());
    auto simulator = parser.parse();
    auto logger = SimulatorLogger<string, char>(verbose, cout);

    try {
        simulator.run(vector<char>(input.begin(), input.end()), &logger);
    } catch (TuringSimulatorException& e) {
        simulator.halt_by_error();
        logger.log_error(simulator, e);
    }
    return 0;
}