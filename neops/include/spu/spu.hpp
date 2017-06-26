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
#ifndef SPU_HPP_INCLUDED
#define SPU_HPP_INCLUDED

#include <cstdint>

#define PSX_SPU_NUM_CREG    20
#define PSX_SPU_CREG_START  0x1f801d80
#define PSX_SPU_CREG_END    0x1f801dbc

namespace spu
{
    void write_creg(std::uint32_t reg, std::uint32_t val);
}

#endif // SPU_HPP_INCLUDED
