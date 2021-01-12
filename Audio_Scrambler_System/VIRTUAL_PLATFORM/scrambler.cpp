#include "scrambler.hpp"
#include <tlm>
#include <tlm_utils/tlm_quantumkeeper.h>

using namespace sc_core;
using namespace sc_dt;
using namespace std;
using namespace tlm;

SC_HAS_PROCESS(scrambler);

scrambler::scrambler(sc_module_name name):
    sc_module(name),
    soc("soc"),
    period(200, SC_NS) {
        soc.register_b_transport(this, & scrambler::b_transport);
    }

void scrambler::b_transport(pl_t & pl, sc_time & offset) {
    tlm_command cmd = pl.get_command();
    uint addr = pl.get_address();
    unsigned char * buf = pl.get_data_ptr();
    unsigned int len = pl.get_data_length();

    switch (cmd) {
    case TLM_WRITE_COMMAND:
        {

            end_of_block++;

            data = * ((sc_uint < TOTAL_NUM_BIT_WIDTH > * ) buf);
            channel_test[end_of_block] = data;

            pl.set_response_status(TLM_OK_RESPONSE);

            if (end_of_block == BLOCK_SIZE) {
                end_of_block = 0;

                //Scrambling algorithm
                int n = 4;
                for (int j = 1; j <= 8192; j++) {
                    int k = 8192 - n + j;

                    if (j % 4 == 0) {
                        n = n + 8;
                    }
                    channel_scrambled[k] = channel_test[j];
                }
                pl.set_response_status(TLM_OK_RESPONSE);
            }
            //The end of scrambling algorithm
        }

        break;
    case TLM_READ_COMMAND:
        {

            memcpy(buf, & channel_scrambled[end_of_block_read], sizeof(channel_scrambled[end_of_block_read]));
            end_of_block_read++;

            if (end_of_block_read == BLOCK_SIZE + 1) {
                end_of_block_read = 1;
            }

            pl.set_response_status(TLM_OK_RESPONSE);
            break;
        }
    default:
        {
            pl.set_response_status(TLM_ADDRESS_ERROR_RESPONSE);
            break;
        }
    }
    offset += sc_time(10, SC_NS);

}
