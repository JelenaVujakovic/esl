#include <systemc>
#include "tb_vp.hpp"
#include "vp.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace sc_core;
using namespace std;

int sc_main(int argc, char** argv)
{


	vp uut("uut");
	char *argv1=argv[1];
	tb_vp tb("tb_vp",&argv1);
	tb.isoc.bind(uut.s_cpu);

	tlm::tlm_global_quantum::instance().set(sc_time(100, SC_NS));

	sc_start();

    return 0;
}
