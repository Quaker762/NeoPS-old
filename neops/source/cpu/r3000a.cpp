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
#include "cpu/r3000a.hpp"

using namespace cpu;

///+++++++++++++++++++UTIL FUNCTIONS!!+++++++++++++++++++//
static std::uint32_t get_rs(std::uint32_t instruction)
{
    return (instruction >> 20) & 0x04;
}

static std::uint32_t get_rt(std::uint32_t instruction)
{
    return (instruction >> 16) & 0x04;
}

static std::uint32_t get_rd(std::uint32_t instruction)
{
    return (instruction >> 10) & 0x04;
}


///+++++++++++++++++++STATIC CPU INSTRUCTIONS!!+++++++++++++++++++//

/**
 *  Add Word.
 *
 *  Format: ADD rd, rs, rt.
 *  Adds two 32-bit values and produces a 32-bit result.
 *
 *  @exception Arithmetic overflow causes an exception.
 */
void r300a_add(r3000a& cpu)
{
    int rs = get_rs(cpu.instruction);
    int rt = get_rt(cpu.instruction);
    int rd = get_rd(cpu.instruction);

    if((cpu.gpr[rs] + cpu.gpr[rt]) > 0x7FFFFFFF)
    {
        cpu.cp0->trigger_exception(cop0::ARITHMETIC_OVERFLOW);
        return; // Addition Does NOT occur!
    }

    cpu.gpr[rd] = cpu.gpr[rs] + cpu.gpr[rt];
}

/**
 *  Add Immediate Word.
 *
 *  Format: ADDI rt, rs, immediate.
 *  16-bit immediate value is sign extended and added to gpr[rs] and stored in gpr[rt].
 *
 *  @exception An overflow exception occurs if carries out of bits 30 and 31 differ (2's compliment overflow).
 */
void r300a_addi(r3000a& cpu)
{
    std::uint32_t imm = (cpu.instruction & 0xFFFF) | 0xFFFF0000; // Sign extend WORD.
    int rt = get_rt(cpu.instruction);
    int rs = get_rs(cpu.instruction);


    if((cpu.gpr[rs] + imm) > 0x7FFFFFFF)
    {
        cpu.cp0->trigger_exception(cop0::ARITHMETIC_OVERFLOW);
        return; // Addition does NOT occur!
    }

    cpu.gpr[rt] = cpu.gpr[rs] + imm;
}

/**
 *  Add Immediate Unsigned Word.
 *
 *  Format: ADDIU rt, rs, immediate.
 *  16-bit immediate value is sign extended and added to gpr[rs] and stored in gpr[rt].
 *
 *  @exception Under no circumstance should an overflow exception be triggered!
 */
void r300a_addiu(r3000a& cpu)
{
    std::uint32_t imm = (cpu.instruction & 0xFFFF) | 0xFFFF0000; // Sign extend WORD.
    int rt = get_rt(cpu.instruction);
    int rs = get_rs(cpu.instruction);

    std::uint32_t temp = (cpu.gpr[rs] + imm);
    cpu.gpr[rt] = temp;
}

/**
 *  Add Unsigned Word.
 *
 *  Format: ADDU rd, rs, rt.
 *  Add two 32-bit values and produce a 32-bit result; arithmetic overflow is ignored (does not cause an exception)..
 *
 *  @exception Under no circumstance should an overflow exception be triggered!
 */
void r300a_addu(r3000a& cpu)
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
void r300a_and(r3000a& cpu)
{
    int rs = get_rs(cpu.instruction);
    int rt = get_rt(cpu.instruction);
    int rd = get_rd(cpu.instruction);

    cpu.gpr[rd] = cpu.gpr[rs] & cpu.gpr[rt];
}

/**
 *  Immediate Bitwise AND
 *
 *  Format: AND rd, rs, rt.
 *  The 16-bit immediate is zero-extended and combined with the contents of general register rs in a bitwise logical AND operation.
 *  The result is placed into general register rt.
 *  @exception None
 */
void r300a_andi(r3000a& cpu)
{
    std::uint32_t imm = (cpu.instruction & 0x0000FFFF); // TODO: Are bits 16-32 zeroed out?
    int rs = get_rs(cpu.instruction);
    int rt = get_rt(cpu.instruction);

    cpu.gpr[rt] = cpu.gpr[rs] & imm;
}

/**
 *  Branch if Equal
 *
 *  Format: BEQ rs, rt, offset
 *  Compares gpr[rs] and gpr[rt]. If they are equal, then the program counter is incremented by offset << 2
 *  @exception None
 */
void r300a_beq(r3000a& cpu)
{
    int rs = get_rs(cpu.instruction);
    int rt = get_rt(cpu.instruction);
    std::uint32_t offset = cpu.instruction & 0x0000FFFF;

    if(cpu.gpr[rs] == cpu.gpr[rt]){}
        // TODO: Weird RISC delay PC...
}

/**
 *  Branch if Greater than or Equal to
 *
 *  Format: BGEZ rs, rt, offset
 *  Compares bit 31 of gpr[rs]. If it is cleared, program branches to (offset << 2) with a delay of 1 instruction.
 *  @exception None
 */
void r300a_bgez(r3000a& cpu)
{
    int rs = get_rs(cpu.instruction);
    std::uint32_t offset = cpu.instruction & 0x0000FFFF;

    if((cpu.gpr[rs] & 0x80000000) == 0){}
        // TODO: Weird RISC delay PC...
}

void r3000a_bgezal(r3000a& cpu)
{
    int rs = get_rs(cpu.instruction);
    std::uint32_t offset = cpu.instruction & 0x0000FFFF;

    cpu.gpr[31] = cpu.pc + 8;

    if((cpu.gpr[rs] & 0x80000000) == 0){}
        // TODO: Weird RISC delay PC...
}

void r3000a_bgtz(r3000a& cpu)
{
    int rs = get_rs(cpu.instruction);
    std::uint32_t offset = cpu.instruction & 0x0000FFFF;

    if((cpu.gpr[rs] & 0x80000000) == 0 && cpu.gpr[rs] != 0){}
        // TODO: Weird RISC delay PC...
}

void r3000a_blez(r3000a& cpu)
{
    int rs = get_rs(cpu.instruction);
    std::uint32_t offset = cpu.instruction & 0x0000FFFF;

    if((cpu.gpr[rs] & 0x80000000) && cpu.gpr[rs] == 0){}
        // TODO: Weird RISC delay PC...
}

void r3000a_bltz(r3000a& cpu)
{
    int rs = get_rs(cpu.instruction);
    std::uint32_t offset = cpu.instruction & 0x0000FFFF;

    if((cpu.gpr[rs] & 0x80000000)){}
        // TODO: Weird RISC delay PC...
}

void r3000a_bltzal(r3000a& cpu)
{

}

void r3000a_bne(r3000a& cpu)
{

}

void r3000a_break(r3000a& cpu)
{
    cpu.cop0->trigger_exception(cop0::BREAKPOINT);
}

///+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

r3000a::r3000a()
{
    cp0 = new cop0();
}

r3000a::~r3000a()
{
    delete cp0;
}








