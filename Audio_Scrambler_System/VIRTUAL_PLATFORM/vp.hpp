#ifndef VP_HPP_INCLUDED
#define VP_HPP_INCLUDED

#include <systemc>
#include <tlm_utils/simple_target_socket.h>
#include <tlm_utils/simple_initiator_socket.h>
#include "sys_bus.hpp"
#include "scrambler.hpp"

class vp:
    sc_core::sc_module
    {
        public:
            vp(sc_core::sc_module_name);
            tlm_utils::simple_target_socket<vp> s_cpu;

        protected:
            tlm_utils::simple_initiator_socket<vp> s_bus;
            sys_bus sb;
            scrambler t;

            typedef tlm::tlm_base_protocol_types::tlm_payload_type pl_t;
            void b_transport(pl_t &, sc_core::sc_time &);
    };

#endif	// VP_HPP_INCLUDED
