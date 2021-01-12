#ifndef TB_VP_HPP_INCLUDED
#define TB_VP_HPP_INCLUDED

#include <sysc/datatypes/fx/sc_fixed.h>
#include <tlm_utils/simple_initiator_socket.h>
#include "typedefs.hpp"

class tb_vp:
    public sc_core::sc_module
    {
        public:
            tb_vp(sc_core::sc_module_name, char **arg);
            tlm_utils::simple_initiator_socket<tb_vp> isoc;

        protected:
            void test();
            char **audiofile;
            typedef uint16_t fixed_point_t;
            sc_dt::sc_uint<TOTAL_NUM_BIT_WIDTH> data;
            double fixed_to_float(fixed_point_t input);
            fixed_point_t float_to_fixed(double input);
            typedef tlm::tlm_base_protocol_types::tlm_payload_type pl_t;
    };

#endif	// TB_VP_HPP_INCLUDED
