ifeq ($(OS),Windows_NT)
	RM := del /Q *.o *.exe
	OUT := .exe
	TESTS := test_simulator
else
	RM := rm -f *.o test turing
	OUT := 
	TESTS := $(shell ls tests/test_*.hpp | sed 's/.*\(test_.*\)\.hpp/\1/')
endif

main:
	g++ -Wall -Wextra -Werror -std=c++17 -o turing$(OUT) src/main.cpp -I src -O2 -g

grammar:
	g++ -Wall -Wextra -Werror -std=c++17 -o turing$(OUT) src/main.cpp -I src -O2
	./turing examples/failure.tm abab -v

# use test_* as test targets, not suitable for Windows
# e.g. tests/test_simulator.hpp -> make test_simulator
$(TESTS):
	g++ -Wall -Wextra -Werror -std=c++17 -o test$(OUT) tests/test.cpp -I src -O2
	./test$(OUT) $@
	$(RM)

clean:
	$(RM)