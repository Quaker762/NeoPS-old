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
#include "bus/psmem.hpp"

#include <cassert>

static std::uint8_t* kuseg = nullptr;

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
    return kuseg[addr];
}

std::uint16_t mem::read_hword(std::uint32_t addr)
{
    std::uint16_t ret = 0;

    ret = (kuseg[addr + 1] << 8) | kuseg[addr];
    return ret;
}

std::uint32_t mem::read_word(std::uint32_t addr)
{
    std::uint16_t ret = 0;

    ret = (kuseg[addr + 3] << 24) | (kuseg[addr + 3] << 16) | (kuseg[addr] << 8) | kuseg[addr];
    return ret;
}
