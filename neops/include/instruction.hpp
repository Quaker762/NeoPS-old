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
#ifndef INSTRUCTION_HPP_INCLUDED
#define INSTRUCTION_HPP_INCLUDED

#include <cstdint>

#pragma pack(push, 1)

/**
 *  32-bit instruction.
 */
typedef union
{
    struct i_type
    {
        std::uint16_t imm;
        std::uint16_t rt : 5;
        std::uint16_t rs : 5;
        std::uint16_t op : 6;
    } i_type;

    struct j_type
    {
        std::uint32_t target : 26;
        std::uint16_t op : 6;
    } j_type;

    struct r_type
    {
        std::uint16_t funct : 6;
        std::uint16_t shamt : 5;
        std::uint16_t rd : 5;
        std::uint16_t rt : 5;
        std::uint16_t rs : 5;
        std::uint16_t op : 6;
    } r_type;

    std::uint32_t instruction;
} instruction_t;


#pragma pack(pop)

#endif // INSTRUCTION_HPP_INCLUDED
