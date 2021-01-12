#include <systemc>
#include<iostream>


int sc_main(int argc, char* argv[]){

	int temp=10;
	time_t t;
	srand((unsigned) time(&t));
	int x=rand()%10;
	while(temp)
	temp--;
	int y=rand()%10;
	if(x>y){
		temp=x;
		x=y;
		y=temp;
	}
	
	sc_start(1+x, sc_core::SC_NS);
	std::cout
		<<"Current sim time 1: "
		<< sc_core::sc_time_stamp()
		<< std::endl;
	sc_start(y-x, sc_core::SC_NS);
	std::cout
		<<"Current sim time 2: "
		<< sc_core::sc_time_stamp()
		<< std::endl;
	sc_start(10-y, sc_core::SC_NS);
	
    return 0;
}
