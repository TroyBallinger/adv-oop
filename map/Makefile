all: test1 test2 morse minimal performance

test1: test1.cpp
	g++ test1.cpp -o test1

test2: test2.cpp
	g++ test2.cpp -o test2

morse: morse.cpp
	g++ morse.cpp -o morse

minimal: Map.hpp minimal.cpp
	g++ minimal.cpp -o minimal

performance: performance.cpp
	g++ -O performance.cpp -o performance

clean:
	rm -f test1 test2 morse minimal performance
