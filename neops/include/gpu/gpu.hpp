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
#ifndef GPU_HPP_INCLUDED
#define GPU_HPP_INCLUDED

#define GPU_GP0_SEND            0x1f801810
#define GPU_GP1_SEND            0x1f801814
#define GPU_GPUREAD_RESPONSE    0x1f801810
#define GPU_GPUREAD_STAT        0x1f801814

namespace gpu
{
    class gpu
    {
    public:

    private:
        std::uint32_t gpustat;
    };

}

#endif // GPU_HPP_INCLUDED
