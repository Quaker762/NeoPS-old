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
#ifndef REGISTER_HPP_INCLUDED
#define REGISTER_HPP_INCLUDED

/**< CPU Register names (NOT Aliases) */
const char* cpu_gpr_names[] = { "R0", "R1", "R2", "R3", "R4", "R5", "R6", "R7",
                                "R8", "R9", "R10", "R11", "R12", "R13", "R14", "R15",
                                "R16", "R17", "R18", "R19", "R20", "R21", "R22", "R23",
                                "R24", "R25", "R26", "R27", "R28", "R29", "R30", "R31"};

/**< CoProcessor 0 (MMU) Register names (NOT Aliases) */
const char* cp0_gpr_names[] = { "cop0r0", "cop0r1", "cop0r2", "cop0r3", "cop0r4", "cop0r5", "cop0r6", "cop0r7",
                                "cop0r8", "cop0r9", "cop0r10", "cop0r11", "cop0r12", "cop0r13", "cop0r14", "cop0r15"
                                "cop0r16", "cop0r17", "cop0r18", "cop0r19", "cop0r20", "cop0r21", "cop0r22", "cop0r23"
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
