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
#include <cstdio>

#include "spu/spu.hpp"

static std::uint32_t spu_creg[PSX_SPU_NUM_CREG]; /**< Our SPU control registers */

using namespace spu;

void spu::write_creg(std::uint32_t reg, std::uint32_t val)
{
    std::uint32_t r_idx = reg - PSX_SPU_CREG_START;
    std::printf("info: writing 0x%08x to spu register %d\n", val, r_idx);

    spu_creg[r_idx] = val;
}
