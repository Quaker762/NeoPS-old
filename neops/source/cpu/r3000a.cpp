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

static inline uint32_t overflow(uint32_t x, uint32_t y, uint32_t z)
{
    return (~(x ^ y) & (x ^ z) & 0x80000000);
}
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
    std::int32_t imm = (std::int16_t)instruction.i_type.imm; // Sign extend immediate value by casting to int16_t
    int rt = instruction.i_type.rt;
    int rs = instruction.i_type.rs;

    std::int32_t rs_val = (std::int32_t)gpr[rs];
    std::uint32_t val = (std::int32_t)rs_val + (std::int32_t)imm;

    if(overflow(rs_val, imm, val))
    {
        cp0->trigger_exception(cop0::ARITHMETIC_OVERFLOW, this);
        std::printf("fatal: ADDI overflow!\n");
        exit(-1);
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
    std::uint32_t imm = (std::int16_t)instruction.i_type.imm;
    int rt = instruction.i_type.rt;
    int rs = instruction.i_type.rs;

    //std::printf("addiu: val = 0x%08x(%d)\n", (std::int16_t)imm, (std::int16_t)imm);
    write_gpr(rt, gpr[rs] + imm);
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

void r3000a::op_bgtz()
{
    std::int16_t target = (std::int16_t)(instruction.i_type.imm << 2);
    int rs = instruction.i_type.rs;

    if(gpr[rs] != 0 && (gpr[rs] & 0x80000000) == 0)
    {
        //std::printf("bgtz: gpr[rs] != 0 && (gpr[rs] & 0x80000000) == 0! setting pc to: 0x%08x\n", pc + target - 4);
        next_pc += target;
        next_pc -= 4;
        is_branch = true;
    }
}

void r3000a::op_blez()
{
    std::int16_t target = (std::int16_t)(instruction.i_type.imm << 2);
    int rs = instruction.i_type.rs;

    if(gpr[rs] == 0 && gpr[rs] & 0x80000000)
    {
        //std::printf("blez: gpr[rs] != 0 && (gpr[rs] & 0x80000000) == 0! setting pc to: 0x%08x\n", pc + target - 4);
        next_pc += target;
        next_pc -= 4;
        is_branch = true;
    }
}

void r3000a::op_bcondz()
{
    int rs = instruction.i_type.rs;
    std::uint32_t is_bgez = (instruction.instruction >> 16) & 1;
    std::uint32_t is_link = ((instruction.instruction >> 17) & 0xf) == 8;
    std::int32_t val = (std::int32_t)gpr[rs];

    std::uint32_t test = (val < 0);

    test = test ^ is_bgez;

    if(is_link)
    {
        write_gpr(31, next_pc);
    }

    if(test != 0)
    {
        std::int16_t target = (std::int16_t)(instruction.i_type.imm << 2);
        //std::printf("bcondz: branching to: 0x%08x\n", pc + target - 4);
        next_pc += target;
        next_pc -= 4;
        is_branch = true;
    }
}

void r3000a::op_beq()
{
    std::int16_t target = (std::int16_t)(instruction.i_type.imm << 2);
    int rs = instruction.i_type.rs;
    int rt = instruction.i_type.rt;

    if(gpr[rs] == gpr[rt])
    {
        //std::printf("beq: gpr[rs] == gpr[rt]! setting pc to: 0x%08x\n", pc + target - 4);
        next_pc += target;
        next_pc -= 4;
        is_branch = true;
    }
}

void r3000a::op_bne()
{
    std::int16_t target = (std::int16_t)(instruction.i_type.imm << 2);
    int rs = instruction.i_type.rs;
    int rt = instruction.i_type.rt;

    if(gpr[rs] != gpr[rt])
    {
        //std::printf("bne: gpr[rs] != gpr[rt]! setting pc to: 0x%08x\n", pc + target - 4);
        next_pc += target;
        next_pc -= 4;
        is_branch = true;
    }
}

void r3000a::op_brk()
{
    cp0->trigger_exception(cop0::BREAKPOINT, this);
}

void r3000a::op_cop0()
{
    if(instruction.instruction & (1 << 25))
    {
        if((instruction.instruction & 0x1ffffff) == 0x10)
        {
            cp0->rfe();
        }

        return;
    }

    int rd = instruction.r_type.rd;
    int rt = instruction.r_type.rt;
    int rs = instruction.r_type.rs;

    if(rs == 0x00)
    {
        std::uint32_t val = cp0->read_gpr(rd);
        std::printf("mfc0: reading cop0 gpr[0x%02x](%s) to cpu gpr[%d] val == 0x%08x\n", rd, cp0_gpr_names[rd], rt, val);
        //gpr[rt] = val;
        load_delay = val;
        delay_reg = rt;
    }
    else if(rs == 0x02)
    {

    }
    else if(rs == 0x04)
    {
        std::uint32_t val = gpr[rt];
        std::printf("mtc0: writing to cop0 gpr[0x%02x] (%s) val == 0x%08x\n", rd, cp0_gpr_names[rd], val);
        cp0->write_gpr(rd, val);
    }
    else if(rs == 0x06)
    {

    }
}

void r3000a::op_cop1()
{
    cp0->trigger_exception(cop0::EXCEPTION_TYPE::COPROCESSOR_UNUSUABLE, this);
}

void r3000a::op_cop2()
{
    std::printf("fatal: unhandled GTE instruction");
    exit(-1);
}

void r3000a::op_cop3()
{
    cp0->trigger_exception(cop0::EXCEPTION_TYPE::COPROCESSOR_UNUSUABLE, this);
}

void r3000a::op_j()
{
    std::uint32_t addr = instruction.j_type.target;

    next_pc = (pc & 0xf0000000) | (addr << 2);
    is_branch = true;
    //std::printf("j: jump called with address 0x%08x! Next instruction at 0x%08x\n", (pc & 0xf0000000) | (addr << 2), next_pc);
}

void r3000a::op_jal()
{
    std::uint32_t addr = instruction.j_type.target;

    write_gpr(31, next_pc);
    //std::printf("jal: jump called with address 0x%08x! Returna address: 0x%08x\n", (pc & 0xf0000000) | (addr << 2), next_pc);
    next_pc = (pc & 0xf0000000) | (addr << 2);
    is_branch = true;
}

void r3000a::op_lb()
{
    if(cp0->read_gpr(0x0c) & 0x00010000)
    {
        //std::printf("lb: ignoring read from cache!\n");
        return;
    }

    std::uint32_t base = instruction.i_type.rs;
    int rt = instruction.i_type.rt;
    std::uint16_t offset = (std::int16_t)instruction.i_type.imm;

    std::uint32_t vaddr = gpr[base] + (std::int16_t)offset;
    std::uint8_t val = (std::int8_t)cp0->virtual_read8(vaddr);
    load_delay = (std::uint32_t)val;
    delay_reg = rt;
}

void r3000a::op_lbu()
{
    if(cp0->read_gpr(0x0c) & 0x00010000)
    {
        //std::printf("lbu: ignoring read from cache!\n");
        return;
    }

    std::uint32_t base = instruction.i_type.rs;
    int rt = instruction.i_type.rt;
    std::int16_t offset = (std::int16_t)instruction.i_type.imm;

    std::uint32_t vaddr = gpr[base] + (std::int16_t)offset;
    std::uint8_t val = cp0->virtual_read8(vaddr);
    write_gpr(rt, val);
}

void r3000a::op_lh()
{
    if(cp0->read_gpr(0x0c) & 0x00010000)
    {
        //std::printf("lh: ignoring read from cache!\n");
        return;
    }

    std::uint32_t base = instruction.i_type.rs;
    int rt = instruction.i_type.rt;
    std::int16_t offset = (std::int16_t)instruction.i_type.imm;

    std::uint32_t vaddr = gpr[base] + (std::int16_t)offset;
    std::int16_t val = (std::int16_t)cp0->virtual_read16(vaddr);
    load_delay = (std::uint32_t)val;
    delay_reg = rt;
}

void r3000a::op_lhu()
{
    if(cp0->read_gpr(0x0c) & 0x00010000)
    {
        //std::printf("lhu: ignoring read from cache!\n");
        return;
    }

    std::uint32_t base = instruction.i_type.rs;
    int rt = instruction.i_type.rt;
    std::int16_t offset = (std::int16_t)instruction.i_type.imm;

    std::uint32_t vaddr = gpr[base] + (std::int16_t)offset;
    std::uint16_t val = cp0->virtual_read16(vaddr);
    load_delay = val;
    delay_reg = rt;
}

void r3000a::op_lui()
{
    int rt = instruction.i_type.rt;
    std::uint32_t imm = instruction.i_type.imm << 16;

    write_gpr(rt, imm);
}

void r3000a::op_lw()
{
    if(cp0->read_gpr(0x0c) & 0x00010000)
    {
        //std::printf("lw: Ignoring read from cache!\n");
        return;
    }

    std::uint32_t base = instruction.i_type.rs;
    int rt = instruction.i_type.rt;
    std::uint32_t offset = (std::int16_t)instruction.i_type.imm;

    std::uint32_t vaddr = offset + gpr[base];
    std::int32_t val = (std::int32_t)cp0->virtual_read32(vaddr);
    //write_gpr(rt, val);
    load_delay = (std::uint32_t)val;
    delay_reg = rt;
}

void r3000a::op_lwc0()
{
    cp0->trigger_exception(cop0::EXCEPTION_TYPE::COPROCESSOR_UNUSUABLE, this);
}

void r3000a::op_lwc1()
{
    cp0->trigger_exception(cop0::EXCEPTION_TYPE::COPROCESSOR_UNUSUABLE, this);
}

void r3000a::op_lwc2()
{
    std::printf("lwc2: invalid load from memory to GTE (not yet implemented)\n");
    exit(-1);
}

void r3000a::op_lwc3()
{
    cp0->trigger_exception(cop0::EXCEPTION_TYPE::COPROCESSOR_UNUSUABLE, this);
}

void r3000a::op_lwl()
{
    if(cp0->read_gpr(0x0c) & 0x00010000)
    {
        //std::printf("lw: Ignoring read from cache!\n");
        return;
    }

    std::uint32_t base = instruction.i_type.rs;
    int rt = instruction.i_type.rt;
    std::uint32_t offset = (std::int16_t)instruction.i_type.imm;

    std::uint32_t vaddr = offset + gpr[base]; // This Virtual Address is _possibly_ unaligned!
    std::uint32_t aligned_val = cp0->virtual_read32((vaddr & (~0x3)));
    std::uint32_t reg_val = gpr_delay[rt];

    std::uint32_t val;

    // Load the left-most bytes
    switch(vaddr & 0x3)
    {
    case 0:
        val = (reg_val & 0x00ffffff) | (aligned_val << 24);
        break;
    case 1:
        val = (reg_val & 0x0000ffff) | (aligned_val << 16);
        break;
    case 2:
        val = (reg_val & 0x000000ff) | (aligned_val << 8);
        break;
    case 3:
        val = (reg_val & 0x00000000) | (aligned_val);
        break;
    default:
        std::printf("lwl: strange address alignment!\n");
        exit(-1);
    }

    write_gpr(rt, val);
}

void r3000a::op_lwr()
{
    if(cp0->read_gpr(0x0c) & 0x00010000)
    {
        //std::printf("lw: Ignoring read from cache!\n");
        return;
    }

    std::uint32_t base = instruction.i_type.rs;
    int rt = instruction.i_type.rt;
    std::uint32_t offset = (std::int16_t)instruction.i_type.imm;

    std::uint32_t vaddr = offset + gpr[base]; // This Virtual Address is _possibly_ unaligned!
    std::uint32_t aligned_val = cp0->virtual_read32((vaddr & (~0x3)));
    std::uint32_t reg_val = gpr_delay[rt];

    std::uint32_t val;

    // Load the left-most bytes
    switch(vaddr & 0x3)
    {
    case 0:
        val = (reg_val & 0x00000000) | (aligned_val);
        break;
    case 1:
        val = (reg_val & 0xff000000) | (aligned_val >> 8);
        break;
    case 2:
        val = (reg_val & 0xffff0000) | (aligned_val >> 16);
        break;
    case 3:
        val = (reg_val & 0xffffffff) | (aligned_val >> 24);
        break;
    default:
        std::printf("lwl: strange address alignment!\n");
        exit(-1);
    }

    write_gpr(rt, val);
}

void r3000a::op_ori()
{
    int rt = instruction.i_type.rt;
    int rs = instruction.i_type.rs;
    std::uint16_t imm = instruction.i_type.imm;

    write_gpr(rt, gpr[rs] | imm);
}

void r3000a::op_sh()
{
    if(cp0->read_gpr(0x0c) & 0x00010000)
    {
        //std::printf("sh: ignoring read from cache!\n");
        return;
    }

    std::uint32_t base = instruction.i_type.rs;
    std::uint32_t rt = instruction.i_type.rt;
    std::uint32_t offset = (std::int16_t)(instruction.i_type.imm);

    std::uint32_t vaddr = gpr[base] + offset;
    cp0->virtual_write16(vaddr, gpr[rt]);
}

void r3000a::op_slti()
{
    std::int32_t imm = (std::int16_t)instruction.i_type.imm;
    int rs = instruction.i_type.rs;
    int rt = instruction.i_type.rt;
    std::int32_t val = gpr[rs];

    if(val < imm)
        write_gpr(rt, 0x00000001);
    else
        write_gpr(rt, 0x00000000);
}

void r3000a::op_sb()
{
    if(cp0->read_gpr(0x0c) & 0x00010000)
    {
        //std::printf("sb: ignoring read from cache!\n");
        return;
    }

    std::uint32_t base = instruction.i_type.rs;
    std::uint32_t rt = instruction.i_type.rt;
    std::uint32_t offset = (std::int16_t)(instruction.i_type.imm);

    std::uint32_t vaddr = gpr[base] + offset;
    cp0->virtual_write8(vaddr, gpr[rt]);
}

void r3000a::op_sw()
{
    if(cp0->read_gpr(0x0c) & 0x00010000)
    {
        //std::printf("sw: ignoring read from cache!\n");
        return;
    }

    std::uint32_t base = instruction.i_type.rs;
    std::uint32_t rt = instruction.i_type.rt;
    std::uint32_t offset = (std::int16_t)(instruction.i_type.imm);

    std::uint32_t vaddr = gpr[base] + offset;
    cp0->virtual_write32(vaddr, gpr[rt]);
}

void r3000a::op_swl()
{
    std::uint32_t base = instruction.i_type.rs;
    std::uint32_t rt = instruction.i_type.rt;
    std::uint32_t offset = (std::int16_t)(instruction.i_type.imm);

    std::uint32_t vaddr = gpr[base] + offset; // This address _may_ be unaligned!
    std::uint32_t aligned_val = cp0->virtual_read32(vaddr & (~0x3));
    std::uint32_t reg_val = gpr[rt];
    std::uint32_t val;

    switch(vaddr & 0x3)
    {
    case 0:
        val = (aligned_val & 0xffffff00) | (reg_val >> 24);
        break;
    case 1:
        val = (aligned_val & 0xffff0000) | (reg_val >> 16);
        break;
    case 2:
        val = (aligned_val & 0xff000000) | (reg_val >> 8);
        break;
    case 3:
        val = (aligned_val & 0x00000000) | (reg_val);
        break;
    default:
        std::printf("swl: strange address alignment!?\n");
        exit(-1);
    }
}

void r3000a::op_swr()
{
    std::uint32_t base = instruction.i_type.rs;
    std::uint32_t rt = instruction.i_type.rt;
    std::uint32_t offset = (std::int16_t)(instruction.i_type.imm);

    std::uint32_t vaddr = gpr[base] + offset; // This address _may_ be unaligned!
    std::uint32_t aligned_val = cp0->virtual_read32(vaddr & (~0x3));
    std::uint32_t reg_val = gpr[rt];
    std::uint32_t val;

    switch(vaddr & 0x3)
    {
    case 0:
        val = (aligned_val & 0x00000000) | (reg_val);
        break;
    case 1:
        val = (aligned_val & 0x000000ff) | (reg_val >> 8);
        break;
    case 2:
        val = (aligned_val & 0x0000ffff) | (reg_val >> 16);
        break;
    case 3:
        val = (aligned_val & 0x00ffffff) | (reg_val >> 24);
        break;
    default:
        std::printf("swr: strange address alignment!?\n");
        exit(-1);
    }
}

void r3000a::op_swc0()
{
    cp0->trigger_exception(cop0::EXCEPTION_TYPE::COPROCESSOR_UNUSUABLE, this);
}

void r3000a::op_swc1()
{
    cp0->trigger_exception(cop0::EXCEPTION_TYPE::COPROCESSOR_UNUSUABLE, this);
}

void r3000a::op_swc2()
{
    std::printf("swc2: invalid write  to GTE (not yet implemented)\n");
    exit(-1);
}

void r3000a::op_swc3()
{
    cp0->trigger_exception(cop0::EXCEPTION_TYPE::COPROCESSOR_UNUSUABLE, this);
}

void r3000a::op_xori()
{
    int rs = instruction.i_type.rs;
    int rt = instruction.i_type.rt;
    std::uint32_t imm = (std::uint32_t)instruction.i_type.imm;

    std::uint32_t val = gpr[rs] ^ imm;
    write_gpr(rt, val);
}

///+++++++++++++++++++++++++++++++SPECIAL OPERATIONS+++++++++++++++++++++++++++++++///

void r3000a::op_add()
{
    int rs = instruction.r_type.rs;
    int rt = instruction.r_type.rt;
    int rd = instruction.r_type.rd;

    std::int32_t rs_val = (std::int32_t)gpr[rs];
    std::int32_t rt_val = (std::int32_t)gpr[rt];
    std::uint32_t val = (std::int32_t)rs_val + (std::int32_t)rt_val;

    if(overflow(rs_val, rt_val, val))
    {
        cp0->trigger_exception(cop0::ARITHMETIC_OVERFLOW, this);
        std::printf("fatal: ADDI overflow!\n");
        return; // Addition does NOT occur!
    }

    write_gpr(rd, val);
}

void r3000a::op_addu()
{
    int rs = instruction.r_type.rs;
    int rt = instruction.r_type.rt;
    int rd = instruction.r_type.rd;

    write_gpr(rd, gpr[rs] + gpr[rt]);
}

void r3000a::op_and()
{
    int rs = instruction.r_type.rs;
    int rt = instruction.r_type.rt;
    int rd = instruction.r_type.rd;

    std::uint32_t val = gpr[rs] & gpr[rt];
    write_gpr(rd, val);
}

void r3000a::op_break()
{
    cp0->trigger_exception(cop0::EXCEPTION_TYPE::BREAKPOINT, this);
}

void r3000a::op_div()
{
    std::int32_t numerator = (std::int32_t)gpr[instruction.r_type.rs];
    std::int32_t divisor = (std::int32_t)gpr[instruction.r_type.rt];

    if(divisor == 0) // Division by zero! That isn't good!!!
    {
        hi = (std::uint32_t)numerator;

        if(numerator > 0)
            lo = 0xffffffff;
        else
            lo = 0x00000001;
    }
    else if((std::uint32_t)numerator == 0x80000000 && divisor == -1)
    {
        hi = 0x00000000;
        lo = 0x80000000;
    }
    else
    {
        hi = (std::uint32_t)(numerator  % divisor);
        lo = (std::uint32_t)(numerator / divisor);
    }
}

void r3000a::op_divu()
{
    int rs = instruction.r_type.rs;
    int rt = instruction.r_type.rt;

    std::uint32_t numerator = gpr[rs];
    std::uint32_t divisor = gpr[rt];

    if(divisor == 0) // Division by zero! That isn't good!!!
    {
        hi = numerator;
        lo = 0xffffffff;
    }
    else
    {
        hi = numerator  % divisor;
        lo = numerator / divisor;
    }
}

void r3000a::op_mfhi()
{
    int rd = instruction.r_type.rd;

    write_gpr(rd, hi);
}


void r3000a::op_mflo()
{
    int rd = instruction.r_type.rd;

    write_gpr(rd, lo);
}

void r3000a::op_mthi()
{
    int rs = instruction.r_type.rs;

    hi = gpr[rs];
}

void r3000a::op_mtlo()
{
    int rs = instruction.r_type.rs;

    lo = gpr[rs];
}

void r3000a::op_mult()
{
    int rs = instruction.r_type.rs;
    int rt = instruction.r_type.rt;
    int rd = instruction.r_type.rd;

    std::uint64_t val = (std::int32_t)gpr[rs] * (std::int32_t)gpr[rt];

    hi = (std::uint32_t)(val >> 32);
    lo = (std::uint32_t)(val & 0x00000000ffffffff);
}

void r3000a::op_multu()
{
    int rs = instruction.r_type.rs;
    int rt = instruction.r_type.rt;
    int rd = instruction.r_type.rd;

    std::uint64_t val = gpr[rs] * gpr[rt];

    hi = (std::uint32_t)(val >> 32);
    lo = (std::uint32_t)(val & 0x00000000ffffffff);
}

void r3000a::op_nor()
{
    int rs = instruction.r_type.rs;
    int rt = instruction.r_type.rt;
    int rd = instruction.r_type.rd;

    std::uint32_t val = ~(gpr[rs] | gpr[rt]);
    write_gpr(rd, val);
}

void r3000a::op_jalr()
{
    int rs = instruction.r_type.rs;

    write_gpr(31, next_pc);
    //std::printf("jalr: Attempting to jump to 0x%08x! Return address: 0x%08x\n", gpr[rs], next_pc);
    next_pc = gpr[rs];
    is_branch = true;
}

void r3000a::op_jr()
{
    int rs = instruction.r_type.rs;

    //std::printf("jr: Attempting to jump to 0x%08x!\n", gpr[rs]);
    next_pc = gpr[rs];
    is_branch = true;
}

void r3000a::op_or()
{
    int rd = instruction.r_type.rd;
    int rs = instruction.r_type.rs;
    int rt = instruction.r_type.rt;

    std::uint32_t val = gpr[rs] | gpr[rt];
    write_gpr(rd, val);
}

void r3000a::op_sll()
{
    int rt = instruction.r_type.rt;
    int rd = instruction.r_type.rd;
    int sh = instruction.r_type.shamt;

    std::uint32_t val = gpr[rt] << sh;
    write_gpr(rd, val);
}

void r3000a::op_sllv()
{
    int rd = instruction.r_type.rd;
    int rs = instruction.r_type.rs;
    int rt = instruction.r_type.rt;

    std::uint32_t val = gpr[rt] << (gpr[rs] & 0x1f);
    write_gpr(rd, val);
}

void r3000a::op_sltiu()
{
    std::uint32_t imm = (std::int16_t)instruction.i_type.imm;
    int rs = instruction.i_type.rs;
    int rt = instruction.i_type.rt;

    if(gpr[rs] < imm)
        write_gpr(rt, 0x00000001);
    else
        write_gpr(rt, 0x00000000);
}

void r3000a::op_slt()
{
    int rd = instruction.r_type.rd;
    int rs = instruction.r_type.rs;

    std::int32_t vs = (std::int32_t)gpr[rs];
    std::int32_t vt = (std::int32_t)gpr[rs];

    if(vs < vt)
        write_gpr(rd, 0x00000001);
    else
        write_gpr(rd, 0x00000000);
}

void r3000a::op_sltu()
{
    int rd = instruction.r_type.rd;
    int rt = instruction.r_type.rt;
    int rs = instruction.r_type.rs;

    if(gpr[rs] < gpr[rt])
        write_gpr(rd, 0x00000001);
    else
        write_gpr(rd, 0x00000000);
}

void r3000a::op_sra()
{
    int rd = instruction.r_type.rd;
    int rt = instruction.r_type.rt;

    std::int32_t val = (std::int32_t)(gpr[rt]) >> instruction.r_type.shamt;
    write_gpr(rd, (std::uint32_t)val);
}

void r3000a::op_srav()
{
    int rd = instruction.r_type.rd;
    int rs = instruction.r_type.rs;
    int rt = instruction.r_type.rt;

    std::uint32_t val = (std::int32_t)(gpr[rt]) >> (gpr[rs] & 0x1f);
    write_gpr(rd, (std::uint32_t)val);
}

void r3000a::op_srl()
{
    int rd = instruction.r_type.rd;
    int rt = instruction.r_type.rt;

    std::uint32_t val = gpr[rt] >> instruction.r_type.shamt;
    write_gpr(rd, val);
}

void r3000a::op_srlv()
{
    int rd = instruction.r_type.rd;
    int rs = instruction.r_type.rs;
    int rt = instruction.r_type.rt;

    std::uint32_t val = gpr[rt] >> (gpr[rs] & 0x1f);
    write_gpr(rd, val);
}

void r3000a::op_subu()
{
    int rd = instruction.r_type.rd;
    int rt = instruction.r_type.rt;
    int rs = instruction.r_type.rs;

    write_gpr(rd, gpr[rs] - gpr[rt]);
}

void r3000a::op_syscall()
{
    cp0->trigger_exception(cop0::EXCEPTION_TYPE::SYSCALL, this);
}

void r3000a::op_xor()
{
    int rd = instruction.r_type.rd;
    int rt = instruction.r_type.rt;
    int rs = instruction.r_type.rs;

    std::uint32_t val = gpr[rs] ^ gpr[rt];
    write_gpr(rd, val);
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
    ops_normal[0x01] = &op_bcondz;
    ops_normal[0x02] = &op_j;
    ops_normal[0x03] = &op_jal;
    ops_normal[0x04] = &op_beq;
    ops_normal[0x05] = &op_bne;
    ops_normal[0x06] = &op_blez;
    ops_normal[0x07] = &op_bgtz;
    ops_normal[0x08] = &op_addi;
    ops_normal[0x09] = &op_addiu;
    ops_normal[0x0a] = &op_slti;
    ops_normal[0x0b] = &op_sltiu;
    ops_normal[0x0c] = &op_andi;
    ops_normal[0x0d] = &op_ori;
    ops_normal[0x0e] = &op_xori;
    ops_normal[0x0f] = &op_lui;
    ops_normal[0x10] = &op_cop0;
    ops_normal[0x11] = &op_cop1;
    ops_normal[0x12] = &op_cop2;
    ops_normal[0x13] = &op_cop3;
    ops_normal[0x20] = &op_lb;
    ops_normal[0x21] = &op_lh;
    ops_normal[0x22] = &op_lwl;
    ops_normal[0x23] = &op_lw;
    ops_normal[0x24] = &op_lbu;
    ops_normal[0x25] = &op_lhu;
    ops_normal[0x26] = &op_lwr;
    ops_normal[0x28] = &op_sb;
    ops_normal[0x29] = &op_sh;
    ops_normal[0x2a] = &op_swl;
    ops_normal[0x2b] = &op_sw;
    ops_normal[0x2e] = &op_swr;
    ops_normal[0x30] = &op_lwc0;
    ops_normal[0x31] = &op_lwc1;
    ops_normal[0x32] = &op_lwc2;
    ops_normal[0x33] = &op_lwc3;
    ops_normal[0x38] = &op_swc0;
    ops_normal[0x39] = &op_swc1;
    ops_normal[0x3a] = &op_swc2;
    ops_normal[0x3b] = &op_swc3;


    ops_special[0x00] = &op_sll;
    ops_special[0x02] = &op_srl;
    ops_special[0x03] = &op_sra;
    ops_special[0x04] = &op_sllv;
    ops_special[0x06] = &op_srl;
    ops_special[0x07] = &op_srav;
    ops_special[0x08] = &op_jr;
    ops_special[0x09] = &op_jalr;
    ops_special[0x0c] = &op_syscall;
    ops_special[0x0d] = &op_break;
    ops_special[0x10] = &op_mfhi;
    ops_special[0x11] = &op_mthi;
    ops_special[0x12] = &op_mflo;
    ops_special[0x13] = &op_mtlo;
    ops_special[0x18] = &op_mult;
    ops_special[0x19] = &op_multu;
    ops_special[0x1a] = &op_div;
    ops_special[0x1b] = &op_divu;
    ops_special[0x20] = &op_add;
    ops_special[0x21] = &op_addu;
    ops_special[0x23] = &op_subu;
    ops_special[0x24] = &op_and;
    ops_special[0x25] = &op_or;
    ops_special[0x26] = &op_xor;
    ops_special[0x27] = &op_nor;
    ops_special[0x2a] = &op_slt;
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
    hi = 0xcafebabe;
    lo = 0xcaf3bab3;
    next_pc = pc + 4;
    is_branch = false;
    delay_slot = false;
    load_delay = 0;
    delay_reg = 0;
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
    gpr[0] = 0x00000000;
}

void r3000a::cycle()
{
    instruction.instruction = cp0->virtual_read32(pc);
    pc = next_pc;
    next_pc += 4;

    delay_slot = is_branch;
    is_branch = false;

    write_gpr(delay_reg, load_delay);
    load_delay = 0;
    delay_reg = 0;

    std::uint32_t opcode = instruction.instruction >> 26;
    //std::printf("(0x%08x): 0x%08x\n", pc - 4, instruction.instruction);

    if(opcode == 0)
    {
        std::uint32_t opcode_s = instruction.instruction & 0x3f;
        if(ops_special[opcode_s] == nullptr)
        {
            std::printf("fatal: unhandled special instruction 0x%08x! Opcode: 0x%02x\n", instruction.instruction, opcode_s);
            exit(-1);
        }

        (this->*ops_special[opcode_s])();
    }
    else
    {
        if(ops_normal[opcode] == nullptr)
        {
            std::printf("fatal: unhandled instruction 0x%08x! Opcode: 0x%02x\n", instruction.instruction, opcode);
            exit(-1);
        }

        (this->*ops_normal[opcode])();
    }

    std::memcpy(gpr, gpr_delay, sizeof(gpr));
}