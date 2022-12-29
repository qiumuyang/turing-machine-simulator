# Turing Machine Simulator

A simple turing machine simulator.

## Reference
Testcases in directory `nijika` comes from [NijikaIjichi](https://github.com/NijikaIjichi/FLA-Testcases)

## Scripts
There are some useful python scripts in directory `pytools`.
+ `formatter.py` helps format turing machine definition file (*.tm).
+ `star_transition_expand.py` replaces all wildcard transitions in .tm (like `s1 * * l s2`) with normal transitions (e.g. `s1 a a l s2` and `s1 b b l s2`).
+ `tester.py` provides basic random-test framework for testing the simulator executable and the .tm file. New testcases can be created easily by deriving from class `Testcase` and implementing `rand_testcase`.
+ `test_all.py` integrates existing testcases and runs them all.
