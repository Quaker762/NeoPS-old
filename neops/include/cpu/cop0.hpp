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
         *  Returns from an interrupt. Restores the "previous" interrupt enable mask bit and kernel/user mode bit
         *
         *  Format: RFE
         *
         *  @exception Coprocessor unusable exception
         */
        void rfe();

        /**
         *  The Index register is loaded with the address of the TLB entry whose contents match the contents of the
         *  EntryHi register. If no TLB entry matches, the high-order bit of Index is set.
         *
         *  Format: TLBP
         *
         *  @exception Coprocessor unusable exception
         */
        void tlbp();
        void tlbr();
        void tlbwi();
        void tlbwr();


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

        /**
         *  Read a byte from memory given a 32-bit virtual address.
         *
         *  @param vaddr - Virtual address.
         *  @return Byte from memory.
         */
        std::uint8_t    virtual_read8(std::uint32_t vaddr);

        /**
         *  Read a half word from memory given a 32-bit virtual address.
         *
         *  @param vaddr - Virtual address.
         *  @return Half Word from memory.
         */
        std::uint16_t   virtual_read16(std::uint32_t vaddr);

        /**
         *  Read a word from memory given a 32-bit virtual address.
         *
         *  @param vaddr - Virtual address.
         *  @return Word from memory.
         */
        std::uint32_t   virtual_read32(std::uint32_t vaddr);

    private:
        std::uint32_t gpr[COP0_MAX_REGS]; /**< 16 32-bit control registers */
        std::uint64_t tlb[COP0_MAX_TLB_ENTRIES]; /**< Our TLB, which contains 64 4kb page entries, which is 256MiB of virtual memory */

        EXCEPTION_TYPE curr_exception;
    };
}

#endif // COP0_HPP_INCLUDED
