#include <systemc>
#include <string>
#include "Module.hpp"
using namespace std;
using namespace sc_core;

int sc_main(int argc, char* argv[])
{
        Module *filozof[5];	
        sc_mutex *fork[5];

	for(int i=0;i<5;i++)
	{
		string str("fiozof_no_");
		string num=to_string(i);
		str=str+num;
		filozof[i]=new Module(str.c_str(), passive, &trigger);
	}

	sc_start(100, sc_core::SC_NS);
	std::cout << "Simulation finished at " << sc_core::sc_time_stamp() << std::endl;
	std::cout << "Number of exploded particles is "<< Module::NumOfExpl<<std::endl;
	return 0;
}
