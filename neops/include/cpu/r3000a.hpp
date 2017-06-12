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
#ifndef R3000_HPP_INCLUDED
#define R3000_HPP_INCLUDED

#include <cstdint>
#include "register.hpp"
#include "cpu/cop0.hpp"

#define GPR_MAX 32 /**< Maximum number of General Purporse Registers (GPRs) contained in the MiPS R3000 */

namespace cpu
{
    /**
     *  Our R3000A CPU. Some interesting quirks and facts:
     *      - No FPU!!!
     *      - 4kb instruction cache.
     *      - Little Endian byte-order.
     *      - 32-bit words, 16-bit half-words and 8-bit bytes.
     *      - Two Co-Processors onboard, cop0: System Control CoProcessor and cop2 the GPU.
     */
    class r3000a
    {
    public:
        r3000a();
        ~r3000a();

        void cycle();

        /**
         *  Write a value to a general purpose register.
         *
         *  @arg reg - Register number we want to write to. Encoded in operation.
         *  @arg value - Value we want to write.
         */
        void write_gpr(unsigned reg, uint32_t value);

        /**
         *  Read a value from a general purpose register.
         *
         *  @arg reg - Register number we want to write to. Encoded in operation.
         */
        std::uint32_t read_gpr(unsigned reg);

        ///////////////////////////////////////////////////////////////////////////////////////////////////

        cop0*           cp0; /**< Our CPU's cop0. */
        //cop2*           co2; /**< Our CPU's cop2. */
        std::uint32_t   gpr[GPR_MAX]; /**< Our General Purprose Registers. */

        std::uint64_t   hi; /**< Multiplication 64 bit high result or division  remainder. */
        std::uint64_t   lo; /**< Multiplication 64 bit low result or division quotient. */
        std::uint32_t   pc; /**< Program Counter. */
        std::uint32_t   instruction; /**< Current instruction. */
    };

}

#endif // R3000_HPP_INCLUDED
