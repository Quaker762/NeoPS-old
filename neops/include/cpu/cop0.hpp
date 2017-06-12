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
#ifndef COP0_HPP_INCLUDED
#define COP0_HPP_INCLUDED

#include <cstdint>

#define COP0_MAX_REGS 16
#define COP0_MAX_TLB_ENTRIES 64

namespace cpu
{
    /**
     *  Our R3000A's first co-processor. Performs various operations in relation to
     *  the system's memory management, system interrupt management (exceptions) and breakpoints.
     */
    class cop0
    {
    public:
        cop0();
        ~cop0();

        enum EXCEPTION_TYPE
        {
            INTERRUPT = 0,
            TLB_MOD,
            TLB_LOAD,
            TLB_STORE,
            ADDRESS_ERROR_LOAD,
            ADDRESS_ERROR_STORE,
            BUS_ERROR_INST,
            BUS_ERROR_DATA,
            SYSCALL,
            BREAKPOINT,
            RESERVED_INSTRUCTION,
            COPROCESSOR_UNUSUABLE,
            ARITHMETIC_OVERFLOW,
        };

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

        void trigger_exception(EXCEPTION_TYPE ex);

        std::uint32_t creg[COP0_MAX_REGS]; /**< 16 32-bit control registers */
        std::uint32_t tlb[COP0_MAX_TLB_ENTRIES]; /**< Our TLB, which contains 64 4kb page entries, which is 256MiB of virtual memory */

        EXCEPTION_TYPE curr_exception;
    };
}

#endif // COP0_HPP_INCLUDED
