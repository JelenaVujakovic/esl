#include "sys_bus.hpp"
#include "vp_addr.hpp"
#include <string>

using namespace std;
using namespace tlm;
using namespace sc_core;
using namespace sc_dt;

sys_bus::sys_bus(sc_module_name name):
    sc_module(name)
    {
        s_cpu.register_b_transport(this, &sys_bus::b_transport);
    }

void sys_bus::b_transport(pl_t &pl, sc_core::sc_time &offset)
{
    uint64 addr = pl.get_address();
    uint64 taddr;
    offset += sc_time(2, SC_NS);

    if (addr == VP_ADDR_SCRAMBLER)
    {
        taddr = addr &0x0000000F;
        pl.set_address(taddr);
        s_scrambler->b_transport(pl, offset);
    }

    pl.set_address(addr);
}
