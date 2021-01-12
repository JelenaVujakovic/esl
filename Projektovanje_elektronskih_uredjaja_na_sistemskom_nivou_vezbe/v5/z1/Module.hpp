#ifndef MODULE_H
#define MODULE_H

#include <systemc>
#include <deque>

SC_MODULE(Module)
{
public:
	SC_HAS_PROCESS(Module);
	Module(sc_core::sc_module_name name, states state, sc_core::sc_mutex *left_fork, sc_core::sc_mutex *right_fork);
	void life();
protected:
	sc_core::sc_mutex *left_fork;
	sc_core::sc_mutex *right_fork;
	bool left_taken=false;
	bool right_taken=false;
};

#endif
