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
#ifndef REGISTER_HPP_INCLUDED
#define REGISTER_HPP_INCLUDED

/**< CPU Register names (NOT Aliases) */
const char* cpu_gpr_names[] = { "zero", "at", "v0", "v1", "a0", "a1", "a2", "a3",
                                "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
                                "t8", "t9", "s0", "s1", "s2", "s3", "s4", "s5",
                                "s6", "s7", "s8", "s9", "gp", "sp", "s8/fp", "ra"};

/**< CoProcessor 0 (MMU) Register names (NOT Aliases) */
const char* cp0_gpr_names[] = { "indx", "rand", "tlbl", "bpc", "ctxt", "bda", "pidmask", "dcic",
                                "badv", "bdam", "tlbh", "bpcm", "sr", "cause", "epc", "prid"
                                "erreg", "cop0r17", "cop0r18", "cop0r19", "cop0r20", "cop0r21", "cop0r22", "cop0r23"
                                "cop0r24", "cop0r25", "cop0r26", "cop0r27", "cop0r28", "cop0r29", "cop0r30", "cop0r31"};

/**
 *  A 32-bit register.
 */
typedef struct
{
    union
    {
        std::uint8_t lo;    /**< Least significant byte of register. */
        std::uint8_t mid1;  /**< Byte 2 of register. */
        std::uint8_t mid2;  /**< Byte 3 of register. */
        std::uint8_t hi;    /**< Most significant byte of register. */
    };

    std::uint32_t word;     /**< Full register word. */
} register32_t;


#endif // REGISTER_HPP_INCLUDED
