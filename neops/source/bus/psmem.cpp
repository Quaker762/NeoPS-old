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

#include "bus/psmem.hpp"
#include "bios.hpp"

static std::uint8_t* kuseg = nullptr;

// IMPORTANT FUCKIN NOTE!!!!
// ALL ADDRESSES ARE PHYSICAL!
void mem::psmem_init()
{
    assert(kuseg == nullptr);

    kuseg = new std::uint8_t[PSX_MEM_SIZE];
}

void mem::psmem_destroy()
{
    assert(kuseg != nullptr);

    delete kuseg;
    kuseg = nullptr;
}

std::uint8_t mem::read_byte(std::uint32_t addr)
{
    if(addr >= PSX_BIOS_SEGMENT_PHYS && addr < PSX_BIOS_SEGMENT_PHYS + PSX_BIOS_SIZE)
        return bios::read_byte(addr - PSX_BIOS_SEGMENT_PHYS);

    std::printf("fatal: invalid read8 of physical address: 0x%08x", addr);
    exit(-1);
}

std::uint16_t mem::read_hword(std::uint32_t addr)
{
    if(addr >= PSX_BIOS_SEGMENT_PHYS && addr < PSX_BIOS_SEGMENT_PHYS + PSX_BIOS_SIZE)
        return bios::read_hword(addr - PSX_BIOS_SEGMENT_PHYS);

    std::printf("fatal: invalid read16 of physical address: 0x%08x", addr);
    exit(-1);
}

std::uint32_t mem::read_word(std::uint32_t addr)
{
    if(addr >= PSX_BIOS_SEGMENT_PHYS && addr < PSX_BIOS_SEGMENT_PHYS + PSX_BIOS_SIZE)
        return bios::read_word(addr - PSX_BIOS_SEGMENT_PHYS);

    std::printf("fatal: invalid read32 of physical address: 0x%08x", addr);
    exit(-1);
}
