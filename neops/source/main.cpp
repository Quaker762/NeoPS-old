#include <iostream>
#include "bus/psmem.hpp"
#include "bios/bios.hpp"
#include "cpu/r3000a.hpp"

int main()
{
    // INITILISATION FUNCTIONS
    mem::psmem_init();
    bios::load_bios("bios/SCPH1001.bin");
    cpu::r3000a cpu;

    bool running = true;

    while(running)
        cpu.cycle();

    return 0;
}
