/*
	Vezbe 1; Zadatak 1: Fibonacijev niz velicine ulaza 64.
*/



#define SC_INCLUDE_FX
#include<systemc>
#include<iostream>
#include<stdlib.h>

int sc_main(int argc, char* argv[])
{
	sc_dt::sc_uint<32> broj=0;	
	sc_dt::sc_uint<43> next=0;
	sc_dt::sc_uint<43> t1=0;
	sc_dt::sc_uint<43> t2=1;
	

	std::cout << "Unesite broj od 0 do 63" << std::endl;
	std::cin >> broj;

	if(64 <= broj){
	
		std::cout << "Niste uneli dobar broj. Unesite broj od 0 do 63" << std::endl;
		std::cin >> broj;
	}

	std::cout << "Uneli ste broj " << broj << "." << std::endl;

	std::cout << "Niz je = ";

	if(broj == 0){
	
		std::cout << "0." << std::endl;
		return 0;
	}

	if(broj == 1){
	
		std::cout << "0, 1." << std::endl;
		return 0;
	}

	std::cout << "0, 1, ";
	

	for (int i = 1; i < broj-1; ++i) {
	
	                next = t1 + t2;
			t1 = t2;
			t2 = next;

			std::cout << next;
			std::cout << ", ";

	}
	        next = t1 + t2;
		t1 = t2;
		t2 = next;
		std::cout << next;


	std::cout << "." << std::endl;


	return 0;

}

