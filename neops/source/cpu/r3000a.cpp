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
#include <cstdio>
#include "cpu/r3000a.hpp"

using namespace cpu;

 const char* cpu_gpr_names[] = { "zero", "at", "v0", "v1", "a0", "a1", "a2", "a3",
                                "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
                                "t8", "t9", "s0", "s1", "s2", "s3", "s4", "s5",
                                "s6", "s7", "s8", "s9", "gp", "sp", "s8/fp", "ra"};

///+++++++++++++++++++STATIC CPU INSTRUCTIONS!!+++++++++++++++++++//

/**
 *  Add Word.
 *
 *  Format: ADD rd, rs, rt.
 *  Adds two 32-bit values and produces a 32-bit result.
 *
 *  @exception Arithmetic overflow causes an exception.
 */
void r3000a::op_add()
{
    int rs = instruction.r_type.rs;
    int rt = instruction.r_type.rt;
    int rd = instruction.r_type.rd;

    if((gpr[rs] + gpr[rt]) > 0x7FFFFFFF)
    {
        cp0->trigger_exception(cop0::ARITHMETIC_OVERFLOW);
        return; // Addition Does NOT occur!
    }

    gpr[rd] = gpr[rs] + gpr[rt];
}

/**
 *  Add Immediate Word.
 *
 *  Format: ADDI rt, rs, immediate.
 *  16-bit immediate value is sign extended and added to gpr[rs] and stored in gpr[rt].
 *
 *  @exception An overflow exception occurs if carries out of bits 30 and 31 differ (2's compliment overflow).
 */
void r3000a::op_addi()
{
    std::uint32_t imm = (std::int16_t)instruction.i_type.imm; // Sign extend immediate value by casting to int16_t
    int rt = imm = instruction.i_type.rt;
    int rs = imm = instruction.i_type.rs;

    if((gpr[rs] + imm) > 0x7FFFFFFF)
    {
        cp0->trigger_exception(cop0::ARITHMETIC_OVERFLOW);
        return; // Addition does NOT occur!
    }

    gpr[rt] = gpr[rs] + imm;
}

/**
 *  Add Immediate Unsigned Word.
 *
 *  Format: ADDIU rt, rs, immediate.
 *  16-bit immediate value is sign extended and added to gpr[rs] and stored in gpr[rt].
 *
 *  @exception Under no circumstance should an overflow exception be triggered!
 */
void r3000a::op_addiu()
{
    std::uint32_t imm = (std::int16_t)instruction.i_type.imm;
    int rt = instruction.i_type.rt;
    int rs = instruction.i_type.rs;

    std::uint32_t temp = gpr[rs] + imm;
    gpr[rt] = temp;
}

/**
 *  Add Unsigned Word.
 *
 *  Format: ADDU rd, rs, rt.
 *  Add two 32-bit values and produce a 32-bit result; arithmetic overflow is ignored (does not cause an exception)..
 *
 *  @exception Under no circumstance should an overflow exception be triggered!
 */
void r3000a::op_addu()
{

}

/**
 *  Bitwise AND
 *
 *  Format: AND rd, rs, rt.
 *  The contents of general register rs are combined with the contents of general register rt in a bit-wise logical AND operation.
 *  The result is placed into general register rd.
 *  @exception None
 */
void r3000a::op_op_and()
{
    int rs = instruction.r_type.rs;
    int rt = instruction.r_type.rt;
    int rd = instruction.r_type.rd;

    gpr[rd] = gpr[rs] & gpr[rt];
}

/**
 *  Immediate Bitwise AND
 *
 *  Format: AND rd, rs, rt.
 *  The 16-bit immediate is zero-extended and combined with the contents of general register rs in a bitwise logical AND operation.
 *  The result is placed into general register rt.
 *  @exception None
 */
void r3000a::op_andi()
{
    std::uint32_t imm = instruction.i_type.imm & 0x0000FFFF; // TODO: Are bits 16-32 zeroed out?
    int rs = instruction.i_type.rs;
    int rt = instruction.i_type.rt;

    gpr[rt] = gpr[rs] & imm;
}

/**
 *  Branch if Equal
 *
 *  Format: BEQ rs, rt, offset
 *  Compares gpr[rs] and gpr[rt]. If they are equal, then the program counter is incremented by offset << 2
 *  @exception None
 */
void r3000a::op_beq()
{
    int rs = instruction.i_type.rs;
    int rt = instruction.i_type.rt;
    std::uint32_t offset = instruction.i_type.imm;

    if(gpr[rs] == gpr[rt]){}
        // TODO: Weird RISC delay PC...
}

/**
 *  Branch if Greater than or Equal to
 *
 *  Format: BGEZ rs, rt, offset
 *  Compares bit 31 of gpr[rs]. If it is cleared, program branches to (offset << 2) with a delay of 1 instruction.
 *  @exception None
 */
void r3000a::op_bgez()
{
    int rs = instruction.i_type.rs;
    std::uint32_t offset = instruction.i_type.imm & 0x0000FFFF;

    if((gpr[rs] & 0x80000000) == 0){}
        // TODO: Weird RISC delay PC...
}

void r3000a::op_bgezal()
{
    int rs = instruction.i_type.rs;
    std::uint32_t offset = instruction.i_type.imm & 0x0000FFFF;

    gpr[31] = pc + 8;

    if((gpr[rs] & 0x80000000) == 0){}
        // TODO: Weird RISC delay PC...
}

void r3000a::op_bgtz()
{
    int rs = instruction.i_type.rs;
    std::uint32_t offset = instruction.i_type.imm & 0x0000FFFF;

    if((gpr[rs] & 0x80000000) == 0 && gpr[rs] != 0){}
        // TODO: Weird RISC delay PC...
}

void r3000a::op_blez()
{
    int rs = instruction.i_type.rs;
    std::uint32_t offset = instruction.i_type.imm & 0x0000FFFF;

    if((gpr[rs] & 0x80000000) && gpr[rs] == 0){}
        // TODO: Weird RISC delay PC...
}

void r3000a::op_bltz()
{
    int rs = instruction.i_type.rs;
    std::uint32_t offset = instruction.i_type.imm & 0x0000FFFF;

    if(gpr[rs] & 0x80000000){}
        // TODO: Weird RISC delay PC...
}

void r3000a::op_bltzal()
{

}

void r3000a::op_bne()
{

}

void r3000a::op_brk()
{
    cp0->trigger_exception(cop0::BREAKPOINT);
}

///+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

r3000a::r3000a()
{
    cp0 = new cop0();

    // Fill instruction jump table
    ops_normal[0x00] = nullptr; // SPECIAL!
    ops_normal[0x04] = &op_beq;
    reset();
}

r3000a::~r3000a()
{
    delete cp0;
}

void r3000a::reset()
{
    //cp0->write_gpr();

    pc = 0xbfc00000; // BIOS location.
    next_instruction.instruction = 0x00;
}

std::uint32_t r3000a::read_gpr(unsigned reg) const
{
    return gpr[reg];
}

void r3000a::write_gpr(unsigned reg, std::uint32_t value)
{
    gpr[reg] = value;
}

void r3000a::cycle()
{
    instruction.instruction = next_instruction.instruction;
    next_instruction.instruction = cp0->virtual_read32(pc);

    std::uint32_t opcode = instruction.instruction >> 26;
    std::printf("instruction: 0x%08x\n", instruction.instruction);

    if(opcode == 0)
    {
        (this->*ops_special[opcode])();
    }
    else
    {
        (this->*ops_normal[opcode])();
    }

    pc += 4;
}








