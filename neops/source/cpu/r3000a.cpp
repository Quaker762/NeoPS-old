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
#include <cstdlib>
#include <cstring>

#include "cpu/r3000a.hpp"
#include "register.hpp"

using namespace cpu;

const char* cpu_gpr_names[] = { "zero", "at", "v0", "v1", "a0", "a1", "a2", "a3",
                                "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
                                "t8", "t9", "s0", "s1", "s2", "s3", "s4", "s5",
                                "s6", "s7", "s8", "s9", "gp", "sp", "s8/fp", "ra"};

///+++++++++++++++++++STATIC CPU INSTRUCTIONS!!+++++++++++++++++++//

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
    std::int32_t imm = (std::int32_t)instruction.i_type.imm; // Sign extend immediate value by casting to int16_t
    int rt = instruction.i_type.rt;
    int rs = instruction.i_type.rs;

    std::uint32_t val = (std::uint32_t)(gpr[rs] + imm);

    if(val > 0x7fffffff)
    {
        cp0->trigger_exception(cop0::ARITHMETIC_OVERFLOW);
        std::printf("fatal: ADDI overflow!\n");
        //exit(-1);
        return; // Addition does NOT occur!
    }

    write_gpr(rt, val);
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
    std::int32_t imm = (std::int32_t)instruction.i_type.imm;
    int rt = instruction.i_type.rt;
    int rs = instruction.i_type.rs;

    write_gpr(rt, (std::int32_t)(gpr[rs] + imm));
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

    write_gpr(rd, gpr[rs] & gpr[rt]);
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
    std::uint32_t imm = instruction.i_type.imm & 0x0000ffff; // TODO: Are bits 16-32 zeroed out?
    int rs = instruction.i_type.rs;
    int rt = instruction.i_type.rt;

    write_gpr(rt, gpr[rs] & imm);
}

void r3000a::op_bne()
{
    std::int16_t target = (std::int16_t)(instruction.i_type.imm << 2);
    int rs = instruction.i_type.rs;
    int rt = instruction.i_type.rt;

    if(gpr[rs] != gpr[rt])
    {
        std::printf("bne: gpr[rs] != gpr[rt]! setting pc to: 0x%08x\n", pc + target - 4);
        pc += target;
        pc -= 4;
    }
}

void r3000a::op_brk()
{
    cp0->trigger_exception(cop0::BREAKPOINT);
}

void r3000a::op_ctc0()
{
    int rt = instruction.r_type.rt;
    int rd = instruction.r_type.rd;

    std::printf("warning: writing to cop0 gpr[0x%02x] (%s)\n", rd, cp0_gpr_names[rd]);
    cp0->write_gpr(rd, gpr[rt]);
}

void r3000a::op_j()
{
    std::uint32_t addr = instruction.j_type.target;

    pc = (pc & 0xf0000000) | (addr << 2);
    std::printf("warning: jump called with address 0x%08x! Next instruction at 0x%08x\n", addr << 2, pc);
}


void r3000a::op_lui()
{
    int rt = instruction.i_type.rt;
    std::uint32_t imm = (instruction.i_type.imm << 16) & 0xffff0000;

    write_gpr(rt, imm);
}

void r3000a::op_lw()
{
    std::uint32_t base = instruction.i_type.rs;
    int rt = instruction.i_type.rt;
    std::int16_t offset = (std::int16_t)instruction.i_type.imm;

    std::uint32_t vaddr = offset + gpr[base];
    gpr[rt] = cp0->virtual_read32(vaddr);
}

void r3000a::op_ori()
{
    int rt = instruction.i_type.rt;
    int rs = instruction.i_type.rs;
    std::uint16_t imm = instruction.i_type.imm;

    write_gpr(rt, gpr[rs] | imm);
}

void r3000a::op_sw()
{
    std::uint32_t base = instruction.i_type.rs;
    std::uint32_t rt = instruction.i_type.rt;
    std::uint32_t offset = (std::int16_t)(instruction.i_type.imm & 0xffff);

    std::uint32_t vaddr = (std::int32_t)(gpr[base] + offset);
    cp0->virtual_write32(vaddr, gpr[rt]);
}

// SPECIAL OPERATIONS
void r3000a::op_or()
{
    int rd = instruction.r_type.rd;
    int rs = instruction.r_type.rs;
    int rt = instruction.r_type.rt;

    write_gpr(rd, gpr[rs] | gpr[rt]);
}

void r3000a::op_sll()
{
    int rt = instruction.r_type.rt;
    int rd = instruction.r_type.rd;
    int sh = instruction.r_type.shamt;

    write_gpr(rd, gpr[rt] << sh);
}

void r3000a::op_sltu()
{
    int rd = instruction.r_type.rd;
    int rt = instruction.r_type.rt;
    int rs = instruction.r_type.rs;

    if(gpr[rs] < gpr[rt])
        write_gpr(rd, (std::uint32_t)0x00000001);
}

///+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

r3000a::r3000a()
{
    cp0 = new cop0();

    for(int i = 0; i < 0x3f; i++)
    {
        ops_normal[i] = nullptr;
        ops_special[i] = nullptr;
    }

    // Fill instruction jump table
//    ops_normal[0x02] = &op_j;
//    ops_normal[0x05] = &op_bne;
//    ops_normal[0x08] = &op_addi;
//    ops_normal[0x09] = &op_addiu;
//    ops_normal[0x10] = &op_ctc0;
//    ops_normal[0x0d] = &op_ori;
//    ops_normal[0x0f] = &op_lui;
//    ops_normal[0x2b] = &op_sw;
//
//    ops_special[0x00] = &op_sll;
//    ops_special[0x25] = &op_or;

    ops_normal[0x02] = &op_j;
    ops_normal[0x05] = &op_bne;
    ops_normal[0x08] = &op_addi;
    ops_normal[0x09] = &op_addiu;
    ops_normal[0x0d] = &op_ori;
    ops_normal[0x0f] = &op_lui;
    ops_normal[0x10] = &op_ctc0;
    ops_normal[0x23] = &op_lw;
    ops_normal[0x2b] = &op_sw;


    ops_special[0x00] = &op_sll;
    ops_special[0x25] = &op_or;
    ops_special[0x2b] = &op_sltu;
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
    std::memset(gpr, 0x00, sizeof(gpr));
    std::memset(gpr_delay, 0x00, sizeof(gpr_delay));
}

std::uint32_t r3000a::read_gpr(unsigned reg) const
{
    return gpr[reg];
}

void r3000a::write_gpr(unsigned reg, std::uint32_t value)
{
    gpr_delay[reg] = value;
}

void r3000a::cycle()
{
    instruction.instruction = next_instruction.instruction;
    next_instruction.instruction = cp0->virtual_read32(pc);

    std::uint32_t opcode = instruction.instruction >> 26;
    std::printf("(0x%08x): 0x%08x\n", pc - 4, instruction.instruction);
    pc += 4;

    if(opcode == 0)
    {
        std::uint32_t opcode_s = instruction.instruction & 0x3f;
        if(ops_special[opcode_s] == nullptr)
        {
            printf("fatal: unhandled special instruction 0x%08x! Opcode: 0x%02x\n", instruction.instruction, opcode_s);
            exit(-1);
        }

        (this->*ops_special[opcode_s])();
    }
    else
    {
        if(ops_normal[opcode] == nullptr)
        {
            printf("fatal: unhandled instruction 0x%08x! Opcode: 0x%02x\n", instruction.instruction, opcode);
            exit(-1);
        }

        (this->*ops_normal[opcode])();
    }

    std::memcpy(gpr, gpr_delay, sizeof(gpr));
}








