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
    along with NeoPS.  If not, see <http://www.gnu.org/licenses/>.
**/
#include "bios.hpp"

#include <fstream>
#include <cassert>
#include <cstdio>

static std::uint8_t* bseg = nullptr;

bool bios::load_bios(const std::string& path)
{
    std::ifstream bfile;
    int bsize;

    assert(bseg == nullptr); // Make sure we don't have a realloc!
    bfile.open(path, std::ios::binary);

    if(bfile.is_open() == false)
    {
        std::printf("bios::load_bios( ): Error opening bios file at %s! Perhaps the file doesn't exist!", path.c_str());
        return false;
    }

    bfile.seekg(0, bfile.end);
    bsize = bfile.tellg();

    if(bsize != PSX_BIOS_SIZE)
    {
        std::printf("bios::load_bios( ): BIOS size is incorrect! Should be %d, read %d!", PSX_BIOS_SIZE, bsize);
        return false;
    }

    bseg = new std::uint8_t[PSX_BIOS_SIZE];

    // Finally, load the BIOS binary.
    bfile.seekg(0, bfile.beg);
    bfile.read((char*)bseg, PSX_BIOS_SIZE);

    return true;
}

std::uint8_t bios::read_byte(std::uint32_t addr)
{
    return bseg[addr];
}

std::uint16_t bios::read_hword(std::uint32_t addr)
{
    std::uint16_t ret = 0;

    ret = (bseg[addr + 1] << 8) | bseg[addr];
    return ret;
}

std::uint32_t bios::read_word(std::uint32_t addr)
{
    std::uint16_t ret = 0;

    ret = (bseg[addr + 3] << 24) | (bseg[addr + 3] << 16) | (bseg[addr] << 8) | bseg[addr];
    return ret;
}