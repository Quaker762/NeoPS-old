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
#ifndef R3000_HPP_INCLUDED
#define R3000_HPP_INCLUDED

#include <cstdint>
#include "cpu/cop0.hpp"
#include "instruction.hpp"

#define R3000_GPR_MAX 32 /**< Maximum number of General Purporse Registers (GPRs) contained in the MiPS R3000 */

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
    typedef void (r3000a::*operation_t)();

    public:
        r3000a();
        ~r3000a();

        /**
         *  Perform one cycle (step) of the CPU.
         */
        void cycle();

        /**
         *  Reset the processor.
         */
        void reset();

        /**
         *  Write a value to a general purpose register.
         *
         *  Note: This writes into the delayed GPR, which are copied after the isntruction has executed.
         *
         *  @arg reg - Register number we want to write to. Encoded in operation.
         *  @arg value - Value we want to write.
         */
        void write_gpr(unsigned reg, std::uint32_t value);

        /**
         *  Read a value from a general purpose register.
         *
         *  @arg reg - Register number we want to write to. Encoded in operation.
         */
        std::uint32_t read_gpr(unsigned reg) const;

        /**
         *  Get the current Program Counter Address (which is a virtual address)
         */
        std::uint32_t get_pc() const
        {
            return pc;
        }

    private:
        cop0*           cp0;                    /**< Our CPU's cp0. */
        //cop2*           co2;                  /**< Our CPU's cop2. */
        std::uint32_t   gpr[R3000_GPR_MAX];     /**< Our General Purprose Registers. */
        std::uint32_t   gpr_delay[R3000_GPR_MAX];

        std::uint64_t   hi;                     /**< Multiplication 64 bit high result or division  remainder. */
        std::uint64_t   lo;                     /**< Multiplication 64 bit low result or division quotient. */
        std::uint32_t   pc;                     /**< Program Counter. */
        std::uint32_t   delay_pc;               /**< Instruction delay PC (of previous instruction)*/
        instruction_t   instruction;            /**< Current instruction. */
        instruction_t   next_instruction;       /**< Branch delay emulation */

        operation_t ops_normal[64];
        operation_t ops_special[64];

        // INSTRUCTIONS
        void op_add();
        void op_addi();
        void op_addiu();
        void op_op_and();
        void op_andi();
        void op_beq();
        void op_bgez();
        void op_bgezal();
        void op_bgtz();
        void op_blez();
        void op_bltz();
        void op_bltzal();
        void op_bne();
        void op_brk();
        void op_ctc0();
        void op_ctc1();
        void op_ctc2();
        void op_ctc3();

        void op_j();
        void op_lui();
        void op_lw();
        void op_ori();
        void op_sh();
        void op_sw();

        // SPECIAL INSTRUCTIONS
        void op_addu();
        void op_or();
        void op_sll();
        void op_sltu();
    };
}

#endif // R3000_HPP_INCLUDED
