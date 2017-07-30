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
    class cop0;

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

        void set_pc(std::uint32_t addr)
        {
            pc = addr;
            next_pc = pc + 4;
        }

    private:
        cop0*           cp0;                    /**< Our CPU's cp0. */
        //cop2*           co2;                  /**< Our CPU's cop2. */
        std::uint32_t   gpr[R3000_GPR_MAX];     /**< Our General Purprose Registers. */
        std::uint32_t   gpr_delay[R3000_GPR_MAX];

        std::uint64_t   hi;                     /**< Multiplication 64 bit high result or division  remainder. */
        std::uint64_t   lo;                     /**< Multiplication 64 bit low result or division quotient. */
        std::uint32_t   epc;                    /**< Current PC value */
        std::uint32_t   pc;                     /**< Program Counter. */
        std::uint32_t   next_pc;                /**< Instruction next PC (of next instruction)*/
        std::uint32_t   load_delay;             /**< Load delay value. */
        std::uint32_t   delay_reg;              /**< Our delay register we want to write to. */
        bool            is_branch;              /**< Has a branch occurred? */
        bool            delay_slot;             /**< Are we in a branch delay?? */
        instruction_t   instruction;            /**< Current instruction. */
        instruction_t   next_instruction;       /**< Next instruction to execute */

        operation_t ops_normal[64];
        operation_t ops_special[64];

        // INSTRUCTIONS
        void op_addi();
        void op_addiu();
        void op_andi();
        void op_bcondz();
        void op_beq();
        void op_bgez();
        void op_bgezal();
        void op_bgtz();
        void op_blez();
        void op_bltz();
        void op_bltzal();
        void op_bne();
        void op_brk();

        void op_j();
        void op_jal();
        void op_jalr();
        void op_lb();
        void op_lbu();
        void op_lh();
        void op_lhu();
        void op_lui();
        void op_lw();
        void op_lwc0();
        void op_lwc1();
        void op_lwc2();
        void op_lwc3();
        void op_lwl();
        void op_lwr();
        void op_cop0();
        void op_cop1();
        void op_cop2();
        void op_cop3();
        void op_ori();
        void op_sb();
        void op_sh();
        void op_sltiu();
        void op_slti();
        void op_sw();
        void op_swl();
        void op_swr();
        void op_swc0();
        void op_swc1();
        void op_swc2();
        void op_swc3();
        void op_xori();


        // SPECIAL INSTRUCTIONS
        void op_add();
        void op_addu();
        void op_and();
        void op_break();
        void op_div();
        void op_divu();
        void op_jr();
        void op_mfhi();
        void op_mflo();
        void op_mthi();
        void op_mtlo();
        void op_mult();
        void op_multu();
        void op_nor();
        void op_or();
        void op_sll();
        void op_slt();
        void op_sltu();
        void op_sllv();
        void op_sra();
        void op_srav();
        void op_srl();
        void op_srlv();
        void op_subu();
        void op_syscall();
        void op_xor();
    };
}

#endif // R3000_HPP_INCLUDED
