#ifndef LOTTO_HPP
#define LOTTO_HPP

#include <systemc>
#include <vector>

typedef std::vector<int> array;

SC_MODULE(lotto)
{
public:
	SC_HAS_PROCESS(lotto);

	lotto(sc_core::sc_module_name name);

	void lotto_combination();
	void first_player();
	void second_player();
	void third_player();
	void print_winner();
	
protected:
	array a;
	int player1;
	int player2;
	int player3;
};

#endif
