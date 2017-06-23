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
#ifndef BIOS_HPP_INCLUDED
#define BIOS_HPP_INCLUDED

#include <string>

#define PSX_BIOS_SIZE 0x80000

namespace bios
{

    /**
     *  Load bios from disk.
     *
     *  @param path - Path to the bios binary.
     *  @return true if loaded successfully, false otherwise
     */
    bool load_bios(const std::string& path);

    /**
     *  Read an 8-bit value from bios.
     *
     *  @param addr - Address we want to read from.
     *  @return Byte from memory.
     */
    std::uint8_t    read_byte(std::uint32_t addr);

    /**
     *  Read a 16-bit value from bios.
     *
     *  @param addr - Address we want to read from.
     *  @return Half-Word from memory.
     */
    std::uint16_t   read_hword(std::uint32_t addr);

    /**
     *  Read a 32-bit value from bios.
     *
     *  @param addr - Address we want to read from.
     *  @return Word from memory.
     */
    std::uint32_t   read_word(std::uint32_t addr);
}

#endif // BIOS_HPP_INCLUDED
