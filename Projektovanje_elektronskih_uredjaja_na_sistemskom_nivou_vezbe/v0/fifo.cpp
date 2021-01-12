#include <systemc>
#include <iostream>

using namespace sc_core;

SC_MODULE(FifoExample)
{
public:
	SC_HAS_PROCESS(FifoExample);

	FifoExample(sc_module_name, int);

protected:
	sc_fifo<int> fifo;

	void source1();
	void source2();
	void drain();
};

FifoExample::FifoExample(sc_module_name name, int size) : sc_module(name), fifo(size)
{
	SC_THREAD(source1);
	SC_THREAD(source2);
	SC_METHOD(drain);
}

void FifoExample::source1()
{
	int s1 = 100;
	while(1)
	{
		wait(rand()%3+3, SC_NS);
		fifo.write(++s1);
		std::cout
			<< "@" << sc_time_stamp() << " source 1 write "
			<< s1 << ".\n";
	}
}

void FifoExample::source2()
{
	int s2 = 300;
	while(1)
	{
		if(fifo.nb_write(s2))
		{
			std::cout
				<< "@" << sc_time_stamp() << " source 2 write "
				<< s2 << ".\n";
		}
		else
		{
			std::cout
				<< "@" << sc_time_stamp() << " fail write.\n";
		}
		wait(rand()%3+3, SC_NS);
	}
}


void FifoExample::drain()
{
	int d;
	for (int i = 0; i < 5; ++i)
		// Blocking read is not allowed in
		// SC_METHOD
		if(fifo.nb_read(d))
		{
			std::cout
				<< "@" << sc_time_stamp() << " read "
				<< d << " data in fifo " << fifo.num_available() << std::endl;
		}
		else
		{
			std::cout
				<< "@" << sc_time_stamp() << " fail write.\n";
			break;
		}

	next_trigger(20, SC_NS);
}

int sc_main(int argc, char* argv[])
{
	const int FIFO_SIZE = 20;
	FifoExample uut("UUT", FIFO_SIZE);

	sc_start(200, SC_NS);

    return 0;
}
