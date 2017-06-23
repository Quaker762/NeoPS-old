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
#ifndef PSMEM_HPP_INCLUDED
#define PSMEM_HPP_INCLUDED

#include <cstdint>

#define PSX_MEM_SIZE 0x200000

/**
 *  Memory and hardware bus.
 *
 *  ALL ADDRESSES USED HERE ARE PHYSICAL ADDRESSES TRANSLATED BY @ref cop0
 *
 *  Memory map as found in psx.pdf by Joshua Walker
 *
 *      kuseg:
 *      0x0000_0000-0x0000_ffff Kernel (64K)
 *      0x0001_0000-0x001f_ffff User Memory (1.9 Meg)
 *
 *      Memory Mapped IO
 *      0x1f00_0000-0x1f00_ffff Parallel Port (64K)
 *      0x1f80_0000-0x1f80_03ff Scratch Pad (1024 bytes)
 *      0x1f80_1000-0x1f80_2fff Hardware Registers (8K)
 *
 *      kseg0
 *      0x8000_0000-0x801f_ffff Kernel and User Memory Mirror (2 Meg) [Cached/TLB]
 *
 *      kesg1
 *      0xa000_0000-0xa01f_ffff Kernel and User Memory Mirror (2 Meg) [Uncached/NO TLB!]
 *
 *      BIOS (real mapped location on a PSX)
 *      0xbfc0_0000-0xbfc7_ffff BIOS (512K)
 */
namespace mem
{

    /**
     *  Initialise the memory subsystem.
     */
    void psmem_init();

    /**
     *  Free up any used memory and release it back to the system.
     */
    void psmem_destroy();

    /**
     *  Read an 8-bit value from address addr
     *
     *  @param addr - Address we want to read from.
     *  @return Byte from memory.
     */
    std::uint8_t    read_byte(std::uint32_t addr);

    /**
     *  Read a 16-bit value from address addr
     *
     *  @param addr - Address we want to read from.
     *  @return Half-Word from memory.
     */
    std::uint16_t   read_hword(std::uint32_t addr);

    /**
     *  Read a 32-bit value from address addr
     *
     *  @param addr - Address we want to read from.
     *  @return Word from memory.
     */
    std::uint32_t   read_word(std::uint32_t addr);
}

#endif // PSMEM_HPP_INCLUDED
