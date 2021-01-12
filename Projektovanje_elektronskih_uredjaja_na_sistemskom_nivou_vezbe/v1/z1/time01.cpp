#include <systemc>
#include <iostream>


int sc_main(int argc, char* argv [])
{
	sc_core::sc_time t1;
	sc_core::sc_time t2(100, sc_core::SC_NS);

	std::cout << "t1 =  " << t1 << std::endl;
	std::cout << "t2 =  " << t2 << std::endl;

	sc_core::sc_time period(5, sc_core::SC_NS);
	sc_core::sc_time delay(4000, sc_core::SC_PS);
	sc_core::sc_time hold(1.2, sc_core::SC_NS);

	sc_core::sc_time skew = period - delay;
	if (skew < hold)
		std::cout <<
			"Time violation: skew = " <<
			skew << std::endl;

	std::cout
		<< "Constant SC_ZERO_TIME = "
		<< sc_core::SC_ZERO_TIME <<  std::endl;

	sc_start(10, sc_core::SC_NS);

	std::cout
		<< "Current sim time = "
		<< sc_core::sc_time_stamp()
		<< std::endl;

    return 0;
}
