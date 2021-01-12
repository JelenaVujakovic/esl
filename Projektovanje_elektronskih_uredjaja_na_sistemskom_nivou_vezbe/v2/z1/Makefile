all : processes_d
processes_d : processes.cpp
	g++ -std=c++11 -I ${SYSTEMC}/include -L${SYSTEMC}/lib-linux64 -lsystemc \-o processes_d processes.cpp
.PHONY: clean
clean:
	rm processes_*
