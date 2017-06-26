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
#include <cassert>
#include <cstring>

#include "bus/psmem.hpp"
#include "bios/bios.hpp"

static std::uint32_t mem_size;          /**< Memory size register. Usually 0x00000b88 */
static std::uint32_t mem_creg[10];      /**< Our memory control registers **/
static std::uint8_t* kuseg = nullptr;   /**< Our base RAM (which is called KUSEG)*/

using namespace mem;

// IMPORTANT FUCKIN NOTE!!!!
// ALL ADDRESSES ARE PHYSICAL!
void mem::psmem_init()
{
    assert(kuseg == nullptr);
    kuseg = new std::uint8_t[PSX_MEM_SIZE];
    std::memset(kuseg, 0xba, sizeof(kuseg));
}

void mem::psmem_destroy()
{
    assert(kuseg != nullptr);

    delete kuseg;
    kuseg = nullptr;
}

void mem::write_creg(std::uint32_t reg, std::uint32_t val)
{
    mem_creg[reg - 0x1f801000] = val;
}

void mem::write_byte(std::uint32_t addr, std::uint8_t val)
{
    std::printf("warning: attempt to write to physical address 0x%08x with val 0x%02x\n", addr, val);
}

void mem::write_hword(std::uint32_t addr, std::uint16_t val)
{
    std::printf("warning: attempt to write to physical address 0x%08x with val 0x%04x\n", addr, val);

    kuseg[addr + 0] = val & 0xff;
    kuseg[addr + 1] = (val >> 8) & 0xff;
    exit(-1);
}

void mem::write_word(std::uint32_t addr, std::uint32_t val)
{
    if(addr >= PSX_MEM_CONTROL_BASE && addr <= PSX_MEM_CONTROL_END)
    {
        write_creg(addr, val);
        return;
    }


    if(addr == PSX_MEM_RAM_SIZE_REG)
    {
        mem_size = val;
        return;
    }

    if(addr == PSX_CACHE_CTRL_REG)
    {
        std::printf("warning: attempt to write cache control with value: 0x%08x!\n", val);
        return;
    }

    std::printf("warning: attempt to write to physical address 0x%08x with val 0x%08x\n", addr, val);

    kuseg[addr + 0] = val & 0xff;
    kuseg[addr + 1] = (val >> 8) & 0xff;
    kuseg[addr + 2] = (val >> 16) & 0xff;
    kuseg[addr + 3] = (val >> 24) & 0xff;
}

std::uint8_t mem::read_byte(std::uint32_t addr)
{
    if(addr >= PSX_BIOS_SEGMENT_PHYS && addr < PSX_BIOS_SEGMENT_PHYS + PSX_BIOS_SIZE)
        return bios::read_byte(addr - PSX_BIOS_SEGMENT_PHYS);

    std::printf("fatal: invalid read8 of physical address: 0x%08x\n", addr);
}

std::uint16_t mem::read_hword(std::uint32_t addr)
{
    if(addr >= PSX_BIOS_SEGMENT_PHYS && addr < PSX_BIOS_SEGMENT_PHYS + PSX_BIOS_SIZE)
        return bios::read_hword(addr - PSX_BIOS_SEGMENT_PHYS);

    std::printf("fatal: invalid read16 of physical address: 0x%08x\n", addr);
}

std::uint32_t mem::read_word(std::uint32_t addr)
{
    if(addr >= PSX_BIOS_SEGMENT_PHYS && addr < PSX_BIOS_SEGMENT_PHYS + PSX_BIOS_SIZE)
        return bios::read_word(addr - PSX_BIOS_SEGMENT_PHYS);

    if(addr >= PSX_MEM_CONTROL_BASE && addr <= PSX_MEM_CONTROL_END)
        return mem_creg[addr];

    return kuseg[addr + 0] | (kuseg[addr + 1] << 8) | (kuseg[addr + 2] << 16) | (kuseg[addr + 3] << 24);
}
