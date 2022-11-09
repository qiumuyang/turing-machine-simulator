ifeq ($(OS),Windows_NT)
	RM := del /Q *.o *.exe
	OUT := .exe
	TESTS := $@
else
	RM := rm -f *.o test turing
	OUT := 
	TESTS := $(shell ls tests/test_*.hpp | sed 's/.*\(test_.*\)\.hpp/\1/')
endif

# use test_* as test targets, not suitable for Windows
# e.g. tests/test_simulator.hpp -> make test_simulator
$(TESTS):
	g++ -Wall -Wextra -Werror -std=c++17 -o test$(OUT) tests/test.cpp -I src
	./test$(OUT) $@
	$(RM)

main:
	g++ -Wall -Wextra -Werror -std=c++17 -o turing$(OUT) src/main.cpp -I src

clean:
	$(RM)