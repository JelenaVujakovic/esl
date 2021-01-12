all:
	g++ -I. -I${SYSTEMC}/include -L. -L${SYSTEMC}/lib-linux64 -Wl,-rpath=${SYSTEMC}/lib-linux64 -o main main.cpp sys_bus.cpp vp.cpp tb_vp.cpp scrambler.cpp -lsystemc -lsndfile -lm
    .PHONY : clean
clean : 
	rm -f main *.out *.txt
