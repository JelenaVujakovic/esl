all: fibonaci_d

fibonaci_d: fibonaci.cpp
	g++ -std=c++11 -I ${SYSTEMC}/include -L${SYSTEMC}/lib-linux64 -lsystemc -o fibonaci_d fibonaci.cpp

.PHONY: clean
clean:
	rm fibonaci_*
