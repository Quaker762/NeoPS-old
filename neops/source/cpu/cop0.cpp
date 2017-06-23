/**
    This file is part of NeoPS.

    NeoPS is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    NeoPS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
**/
#include <cstdio>
#include <cstdlib>

#include "cpu/cop0.hpp"
#include "bus/psmem.hpp"

#define KSEG0 0x80000000
#define KSEG1 0xa0000000

const char* cp0_gpr_names[] = { "indx", "rand", "tlbl", "bpc", "ctxt", "bda", "pidmask", "dcic",
                                "badv", "bdam", "tlbh", "bpcm", "sr", "cause", "epc", "prid"
                                "erreg", "cop0r17", "cop0r18", "cop0r19", "cop0r20", "cop0r21", "cop0r22", "cop0r23"
                                "cop0r24", "cop0r25", "cop0r26", "cop0r27", "cop0r28", "cop0r29", "cop0r30", "cop0r31"};

using namespace cpu;

cop0::cop0()
{

}

cop0::~cop0()
{

}

void cop0::trigger_exception(EXCEPTION_TYPE ex)
{

}

// TODO: Caching???
std::uint8_t cop0::virtual_read8(std::uint32_t vaddr)
{
    if(vaddr % 4 != 0)
    {
        std::printf("fatal: attempt to read from unalined memory address! (SIGBUS)\n");
        exit(-1);
    }

    if(vaddr & KSEG0) // This virtual address is mapped to KSEG0
        return mem::read_byte(vaddr - KSEG0);
    else if(vaddr & KSEG1)  // This virtual address is mapped to KESG1
        return mem::read_byte(vaddr - KSEG1);

    return mem::read_byte(vaddr);
}

// TODO: Caching???
std::uint16_t cop0::virtual_read16(std::uint32_t vaddr)
{
    if(vaddr % 4 != 0)
    {
        std::printf("fatal: attempt to read from unalined memory address! (SIGBUS)\n");
        exit(-1);
    }

    if(vaddr & KSEG0) // This virtual address is mapped to KSEG0
        return mem::read_hword(vaddr - KSEG0);
    else if(vaddr & KSEG1)  // This virtual address is mapped to KESG1
        return mem::read_hword(vaddr - KSEG1);

    return mem::read_hword(vaddr);
}

// TODO: Caching???
std::uint32_t cop0::virtual_read32(std::uint32_t vaddr)
{
    if(vaddr % 4 != 0)
    {
        std::printf("fatal: attempt to read from unalined memory address! (SIGBUS)\n");
        exit(-1);
    }

    if(vaddr & KSEG0) // This virtual address is mapped to KSEG0
        return mem::read_word(vaddr - KSEG0);
    else if(vaddr & KSEG1)  // This virtual address is mapped to KESG1
        return mem::read_word(vaddr - KSEG1);

    return mem::read_word(vaddr);
}
