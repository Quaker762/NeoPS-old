#include <iostream>
#include "bus/psmem.hpp"
#include "bios.hpp"
#include "cpu/r3000a.hpp"

int main()
{
    // INITILISATION FUNCTIONS
    mem::psmem_init();
    bios::load_bios("bios/SCPH1001.bin");
    cpu::r3000a cpu;

    return 0;
}
