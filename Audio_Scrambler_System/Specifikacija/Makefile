all:
	g++ -I. -I${SYSTEMC_HOME}/include -L. -L${SYSTEMC_HOME}/lib-linux64 -Wl,-rpath=${SYSTEMC_HOME}/lib-linux64 -o main main.cpp -lsystemc -lsndfile -lm
clean : 
	rm -f main *.out *.txt
