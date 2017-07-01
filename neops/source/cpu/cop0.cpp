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
#include "bus/bus.hpp"
#include "register.hpp"

#define KSEG0 0b100
#define KSEG1 0b101
#define KSEG2 0b111

#define KSEG0_MASK 0x80000000
#define KSEG1_MASK 0xa0000000
#define KSEG2_MASK 0xfffe0000

const char* cp0_gpr_names[] = { "indx", "rand", "tlbl", "bpc", "ctxt", "bda", "pidmask", "dcic",
                                "badv", "bdam", "tlbh", "bpcm", "sr", "cause", "epc", "prid"
                                "erreg", "cop0r17", "cop0r18", "cop0r19", "cop0r20", "cop0r21", "cop0r22", "cop0r23"
                                "cop0r24", "cop0r25", "cop0r26", "cop0r27", "cop0r28", "cop0r29", "cop0r30", "cop0r31"};

static inline bool align_check(std::uint32_t addr)
{
    return addr % 4 == 0 || addr % 2 == 0;
}

static std::uint32_t address_masks[] =  {0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, // kuseg
                                         0x7fffffff, // kseg0
                                         0x1fffffff, // kseg1
                                         0xffffffff, 0xffffffff
                                        };

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

void cop0::write_gpr(unsigned reg, std::uint32_t val)
{
    gpr[reg] = val;
}

std::uint32_t cop0::read_gpr(unsigned reg)
{
    return gpr[reg];
}

void cop0::virtual_write8(std::uint32_t vaddr, std::uint8_t value)
{
    int segment = vaddr >> 29;
    std::uint32_t phys_addr = vaddr & address_masks[segment];

    mem::write_byte(phys_addr, value);
}

void cop0::virtual_write16(std::uint32_t vaddr, std::uint16_t value)
{
    // TODO: BUS ERROR TRIGGER
    if(!align_check(vaddr))
    {
        std::printf("fatal: attempt to write from unaligned memory address! (SIGBUS)\n");;
        exit(-1);
    }

    int segment = vaddr >> 29;
    std::uint32_t phys_addr = vaddr & address_masks[segment];
    mem::write_hword(phys_addr, value);
}

void cop0::virtual_write32(std::uint32_t vaddr, std::uint32_t value)
{
    // TODO: BUS ERROR TRIGGER
    if(!align_check(vaddr))
    {
        std::printf("fatal: attempt to write to unaligned memory address: 0x%08x (SIGBUS)\n", vaddr);
        exit(-1);
    }

    int segment = vaddr >> 29;
    std::uint32_t phys_addr = vaddr & address_masks[segment];

    mem::write_word(phys_addr, value);
}

// TODO: Caching???
std::uint8_t cop0::virtual_read8(std::uint32_t vaddr)
{
    int segment = vaddr >> 29;
    std::uint32_t phys_addr = vaddr & address_masks[segment];

    return mem::read_byte(phys_addr);
}

// TODO: Caching???
std::uint16_t cop0::virtual_read16(std::uint32_t vaddr)
{
    if(!align_check(vaddr))
    {
        std::printf("fatal: attempt to read from unalined memory address: 0x%08x (SIGBUS)\n", vaddr);
        exit(-1);
    }

    int segment = vaddr >> 29;
    std::uint32_t phys_addr = vaddr & address_masks[segment];

    return mem::read_hword(phys_addr);
}

// TODO: Caching???
std::uint32_t cop0::virtual_read32(std::uint32_t vaddr)
{
    if(!align_check(vaddr))
    {
        std::printf("fatal: attempt to read from unalined memory address: 0x%08x (SIGBUS)\n", vaddr);
        exit(-1);
    }

    int segment = vaddr >> 29;
    std::uint32_t phys_addr = vaddr & address_masks[segment];

    return mem::read_word(phys_addr);
}
