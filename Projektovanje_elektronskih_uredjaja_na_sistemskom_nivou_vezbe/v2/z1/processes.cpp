#include <systemc>
#include <iostream>

enum stanje { s_0, s_1, s_2, s_3 };

using namespace sc_core;

SC_MODULE(Proc)
{
public:
	SC_HAS_PROCESS(Proc);

	Proc(sc_module_name);

protected:
	void print(const char *str);

	sc_event clk, a, b;
	void clk_thread();
	void ab_thread();
	void dynamic_method();
		int g;
	int ab_int;
	stanje s=s_0;
};

Proc::Proc(sc_module_name name) : sc_module(name)
{
	SC_THREAD(clk_thread);
	SC_THREAD(ab_thread);
	sensitive << a << b;
	SC_METHOD(dynamic_method);
	sensitive << a << b;
	ab_int = 0;
}

void Proc::print(const char *str)
{
	std::cout <<
		"@ " <<
		sc_time_stamp() << " " <<
		str <<
		std::endl;
}

void Proc::clk_thread()
{
	while(1)
	{
		wait(5, SC_NS);
		clk.notify();
	}
}

void Proc::ab_thread()
{
	while(1)
	{
		wait(5, SC_NS);
		a.notify();
		g=3;
		wait(5, SC_NS);
		b.notify();
		g=2;
	}
}


void Proc::dynamic_method()
{
	sc_time d(3, SC_NS);
	switch(s)
	{
	case s_0:
		print("Idle");
		next_trigger(d, a | b);
		s=s_1;
		break;
	case s_1:
		print("Step 1 ");
		next_trigger(a | b);	
		switch(g){	
		case 2:
		s=s_2;	
		break;			
		case 3:	
 		
		s=s_3;
		break; 	}
		break;
	case s_2:
		print("Step 2");
		next_trigger(d);
		s=s_0;
		break;
	case s_3:
		print("Step 3");
		next_trigger(d);
		s=s_0;
		break;
	default:
		assert(false);
	}
	ab_int = (ab_int + 1) % 4;
}

int sc_main(int argc, char* argv[])
{
	Proc uut("UUT");

	sc_start(100, SC_NS);

    return 0;
}
