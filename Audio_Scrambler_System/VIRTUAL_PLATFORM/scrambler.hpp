#ifndef SCRAMBLER_HPP_INCLUDED
#define SCRAMBLER_HPP_INCLUDED
#include <tlm>
#include <tlm_utils/simple_target_socket.h>
#include <sysc/datatypes/fx/sc_fixed.h>
#include "typedefs.hpp"

class scrambler: public sc_core::sc_module
{
    public:
        scrambler(sc_core::sc_module_name);
        tlm_utils::simple_target_socket<scrambler> soc;
        inline void set_period(sc_core::sc_time &);

    protected:
        unsigned char *b;
        sc_dt::sc_uint<TOTAL_NUM_BIT_WIDTH> data;
        int end_of_block = 0;
        int end_of_block_read = 1;
        sc_core::sc_time period;
        typedef tlm::tlm_base_protocol_types::tlm_payload_type pl_t;
        typedef sc_dt::sc_fixed<TOTAL_NUM_BIT_WIDTH, 2> num_t;
        sc_dt::sc_uint<TOTAL_NUM_BIT_WIDTH> channel_test[BLOCK_SIZE], channel_scrambled[BLOCK_SIZE],
            channel_scrambled1[BLOCK_SIZE];
        unsigned char ram[RAM_SIZE];
        void b_transport(pl_t &, sc_core::sc_time &);
        int sik = 0;
};

void scrambler::set_period(sc_core::sc_time &t)
{
    period = t;
}

#endif	// SCRAMBLER_HPP_INCLUDED
