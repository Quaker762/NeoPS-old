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
#include <cassert>
#include <cstring>

#include "bus/bus.hpp"
#include "bios/bios.hpp"
#include "dma/dma.hpp"
#include "gpu/gpu.hpp"
#include "spu/spu.hpp"

static std::uint32_t mem_size;          /**< Memory size register. Usually 0x00000b88 */
static std::uint32_t mem_creg[10];      /**< Our memory control registers **/
static std::uint8_t* kuseg = nullptr;   /**< Our base RAM (which is called KUSEG)*/

using namespace bus;

dma_controller dma;

// IMPORTANT FUCKIN NOTE!!!!
// ALL ADDRESSES ARE PHYSICAL!
void bus::psmem_init()
{
    assert(kuseg == nullptr);
    kuseg = new std::uint8_t[PSX_MEM_SIZE];
    std::memset(kuseg, 0xba, PSX_MEM_SIZE);
}

void bus::psmem_destroy()
{
    assert(kuseg != nullptr);

    delete kuseg;
    kuseg = nullptr;
}

void bus::write_creg(std::uint32_t reg, std::uint32_t val)
{
    mem_creg[reg - 0x1f801000] = val;
}

void bus::write_byte(std::uint32_t addr, std::uint8_t val)
{
    //std::printf("write_byte: attempt to write to physical address 0x%08x with val 0x%02x\n", addr, val);

    if(addr >= 0x1f802000 && addr <= 0x1f802042)
    {
        std::printf("Attempt to write to Expansion2! 0x%08x:0x%08x\n", addr, val);
        return;
    }

    if(addr == 0x1f801800)
    {
        std::printf("cdrom: attempt to write CDROM register!\n");
        exit(-1);
    }

    kuseg[addr] = val;
}

void bus::write_hword(std::uint32_t addr, std::uint16_t val)
{
    //std::printf("write_hword: attempt to write to physical address 0x%08x with val 0x%04x\n", addr, val);

    if(addr >= PSX_SPU_CREG_START && addr <= PSX_SPU_CREG_END)
    {
        spu::write_creg(addr, val);
        return;
    }

    if(addr == PSX_TIMER_MODE_0 || addr == PSX_TIMER_MODE_1 || addr == PSX_TIMER_MODE_2)
    {
        std::printf("warning: attempt to write 0x%04x to timer register 0x%08x!\n", val, addr);
        return;
    }

    if(addr == PSX_TIMER_TARGET_0 || addr == PSX_TIMER_TARGET_1 || addr == PSX_TIMER_TARGET_2)
    {
        std::printf("warning: attempt to write 0x%04x to timer register 0x%08x!\n", val, addr);
        return;
    }

    if(addr == PSX_TIMER_COUNTER_0 || addr == PSX_TIMER_COUNTER_1 || addr == PSX_TIMER_COUNTER_2)
    {
        std::printf("warning: attempt to write 0x%04x to timer register 0x%08x!\n", val, addr);
        return;
    }

    if(addr >= 0x1f801c00 && addr <= 0x1f801e80)
    {
        std::printf("attempt to write spu with value 0x%08x!\n", val);
        return;
    }

    if(addr == PSX_INTERRUPT_MASK_REG)
    {
        std::printf("warning: attempt 16-bit write of PSX_INTERRUPT_MASK_REG!\n");
        return;
    }

    if(addr >= DMA_CHANNEL0_BASE && addr <= DMA_CHANNEL6_BASE + 8)
    {
        std::printf("dma: attempting to write 0x%08x to DMA register 0x%08x\n", val, addr);
        dma.controller_write(addr, val);
        return;
    }

    kuseg[addr + 0] = val & 0xff;
    kuseg[addr + 1] = (val >> 8) & 0xff;
}

void bus::write_word(std::uint32_t addr, std::uint32_t val)
{
    //std::printf("write_word: attempt to write to physical address 0x%08x with val 0x%08x\n", addr, val);

    if(addr == 0x1f801074 || addr == 0x1f801070)
    {
        std::printf("warning: attempt to write IRQ register!\n");
        return;
    }

    if(addr >= PSX_MEM_CONTROL_BASE && addr <= PSX_MEM_CONTROL_END)
    {
        write_creg(addr, val);
        return;
    }

    if(addr == PSX_MEM_RAM_SIZE_REG)
    {
        mem_size = val;
        return;
    }

    if(addr == PSX_CACHE_CTRL_REG)
    {
        std::printf("warning: attempt to write cache control with value: 0x%08x!\n", val);
        return;
    }

    if(addr == GPU_GP0_SEND)
    {
        //std::printf("gp0: attempt to send command/packet to gp0! value: 0x%08x\n", val);
        return;
    }

    if(addr == GPU_GP1_SEND)
    {
        //std::printf("gp1: attempt to send command/packet to gp0! value: 0x%08x\n", val);
        return;
    }

    if(addr == PSX_TIMER_MODE_0 || addr == PSX_TIMER_MODE_1 || addr == PSX_TIMER_MODE_2)
    {
        std::printf("warning: attempt to write 0x%08x to timer register 0x%08x!\n", val, addr);
        return;
    }

    if(addr == PSX_TIMER_TARGET_0 || addr == PSX_TIMER_TARGET_1 || addr == PSX_TIMER_TARGET_2)
    {
        std::printf("warning: attempt to write 0x%08x to timer register 0x%08x!\n", val, addr);
        return;
    }

    if(addr == PSX_TIMER_COUNTER_0 || addr == PSX_TIMER_COUNTER_1 || addr == PSX_TIMER_COUNTER_2)
    {
        std::printf("warning: attempt to write 0x%08x to timer register 0x%08x!\n", val, addr);
        return;
    }

    if(addr == DMA_CTRL_REG)
    {
        //std::printf("warning: attempt to write DMA control reg (0x%08x) with value: 0x%08x!\n", addr, val);
        dma.write_dpcr(val);
        return;
    }

    if(addr == DMA_INTERRUPT_REG)
    {
        //std::printf("warning: attempt to write DMA interrupt reg (0x%08x) with value: 0x%08x!\n", addr, val);
        dma.write_dicr(val);
        return;
    }

    if(addr >= DMA_CHANNEL0_BASE && addr <= DMA_CHANNEL6_BASE + 8)
    {
        std::printf("dma: attempting to write 0x%08x to DMA register 0x%08x\n", val, addr);
        dma.controller_write(addr, val);
        return;
    }

    kuseg[addr + 0] = val & 0xff;
    kuseg[addr + 1] = (val >> 8) & 0xff;
    kuseg[addr + 2] = (val >> 16) & 0xff;
    kuseg[addr + 3] = (val >> 24) & 0xff;
}

std::uint8_t bus::read_byte(std::uint32_t addr)
{
    if(addr >= PSX_BIOS_SEGMENT_PHYS && addr <= PSX_BIOS_SEGMENT_PHYS + PSX_BIOS_SIZE)
        return bios::read_byte(addr - PSX_BIOS_SEGMENT_PHYS);

    if(addr >= 0x1f802000 && addr <= 0x1f802042)
    {
        std::printf("Attempt to read Expansion2! 0x%08x\n", addr);
        return 0xFF;
    }

    if(addr >= 0x1f000080 && addr <= 0x1f000084)
    {
        std::printf("Attempt to read Expansion1! 0x%08x\n", addr);
        return 0xFF;
    }

    return kuseg[addr];
}

std::uint16_t bus::read_hword(std::uint32_t addr)
{
    if(addr >= PSX_BIOS_SEGMENT_PHYS && addr < PSX_BIOS_SEGMENT_PHYS + PSX_BIOS_SIZE)
        return bios::read_hword(addr - PSX_BIOS_SEGMENT_PHYS);

    if(addr >= PSX_SPU_CREG_START && addr <= PSX_SPU_CREG_END)
    {
        //std::printf("warning: attempt to read spu register! Ignoring...\n");
        return 0x00;
    }

    if(addr >= 0x1f801c00 && addr <= 0x1f801e80)
    {
        std::printf("attempt to read spu 0x%08x\n", addr);
        return 0x00;
    }

    if(addr == PSX_INTERRUPT_MASK_REG)
    {
        std::printf("warning: attempt 16-bit read of PSX_INTERRUPT_MASK_REG!\n");
        return 0x00;
    }

    return kuseg[addr] | (kuseg[addr + 1] << 8);
}

std::uint32_t bus::read_word(std::uint32_t addr)
{
    if(addr >= PSX_BIOS_SEGMENT_PHYS && addr < PSX_BIOS_SEGMENT_PHYS + PSX_BIOS_SIZE)
        return bios::read_word(addr - PSX_BIOS_SEGMENT_PHYS);

    if(addr >= PSX_MEM_CONTROL_BASE && addr <= PSX_MEM_CONTROL_END)
        return mem_creg[addr];

    if(addr == PSX_INTERRUPT_MASK_REG)
    {
        std::printf("warning: attempt to read from PSX_INTERRUPT_MASK_REG! Returning 0 for now!\n");
        return 0;
    }


    if(addr == GPU_GPUREAD_RESPONSE)
    {
        return 0x00;
    }

    if(addr == GPU_GPUREAD_STAT)
    {
        //std::printf("attempt to read GPUSTAT!\n");
        return 0x1c000000;
    }

    if(addr == DMA_CTRL_REG)
    {
        //std::printf("warning: reading DMA control register\n");
        return dma.read_dpcr();
    }

    if(addr == DMA_INTERRUPT_REG)
    {
        //std::printf("warning: attempt to read from DMA interrupt register! Ignoring...\n");
        return dma.read_dicr();
    }


    if(addr >= DMA_CHANNEL0_BASE && addr <= DMA_CHANNEL6_BASE + 8)
    {
        //std::printf("dma: attempting read of DMA register 0x%08x\n", addr);
        return dma.controller_read(addr);
    }

    if(addr == PSX_TIMER_COUNTER_0 || addr == PSX_TIMER_COUNTER_1 || addr == PSX_TIMER_COUNTER_2)
    {
        //std::printf("warning: attempt to read timer counter 0x%08x!\n", addr);
        return 0x00;
    }

    if(addr == 0x1F8010F8)
        return DMA_1F8010F8H;

    if(addr == 0x1F8010FC)
        return DMA_1F8010FCh;

    return kuseg[addr + 0] | (kuseg[addr + 1] << 8) | (kuseg[addr + 2] << 16) | (kuseg[addr + 3] << 24);
}
