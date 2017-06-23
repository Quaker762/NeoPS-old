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
extern const char* cpu_gpr_names[];

/**< CoProcessor 0 (MMU) Register names (NOT Aliases) */
extern const char* cp0_gpr_names[];

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
