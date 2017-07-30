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
#include "dma/dma.hpp"
#include "bus/bus.hpp"

#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace bus;

dma_controller::dma_controller()
{
    std::memset(&channels, 0x00, sizeof(channel) * 7);
    dpcr = 0x07654321;
}

dma_controller::~dma_controller()
{

}

void dma_controller::write_dpcr(std::uint32_t val)
{
    dpcr = val;
}

std::uint32_t dma_controller::read_dpcr()
{
    return dpcr;
}

void dma_controller::write_dicr(std::uint32_t val)
{
    dicr &= 0xff000000;
    dicr |= (val & 0x00ff803f);
    dicr &= ~(val & 0x7f000000);
    update_irq_active();
}

std::uint32_t dma_controller::read_dicr()
{
    return dicr;
}

void dma_controller::update_irq_active()
{
    bool forced = ((dicr >> 15) & 1) != 0;
    bool master = ((dicr >> 23) & 1) != 0;
    bool signal = ((dicr >> 16) & (dicr >> 24) & 0x7f) != 0;
    bool active = forced || (master && signal);

    if(active)
    {
        if(!(dicr & 0x80000000))
        {

        }

        dicr |= 0x80000000;
    }
    else
    {
        dicr &= ~0x80000000;
    }
}

std::uint32_t dma_controller::controller_read(std::uint32_t address)
{
    std::uint32_t channel;
    std::uint32_t reg;

    channel = ((address >> 4) & 0x0F) - 8;
    reg  = address & 0x0f;

    switch(reg)
    {
    case 0:
        return channels[channel].base_address;
    case 4:
        return channels[channel].block_control;
    case 8:
        return channels[channel].channel_control;
    default:
        std::printf("controller_read: invalid offset, %d!\n", reg);
        exit(-1);
    }
}

void dma_controller::controller_write(std::uint32_t address, std::uint32_t val)
{
    std::uint32_t channel;
    std::uint32_t reg;

    channel = ((address >> 4) & 0x0F) - 8;
    reg  = address & 0x0f;

    if(reg == 0)
    {
        channels[channel].base_address = val;
    }
    else if(reg == 4)
    {
        channels[channel].block_control = val;
    }
    else if(reg == 8)
    {
        channels[channel].channel_control = val;

        // Set up transfer direction
        if(val & 0x01)
            channels[channel].direction = DIRECTION::FROM_RAM;
        else
            channels[channel].direction = DIRECTION::TO_RAM;

        std::uint32_t mode = (val >> 9) & 0x03;
        // Set up SyncMode
        switch(mode)
        {
        case 0:
            channels[channel].syncmode = SYNC_MODE::IMMEDIATE;
            break;
        case 1:
            channels[channel].syncmode = SYNC_MODE::REQUEST;
            break;
        case 2:
            channels[channel].syncmode = SYNC_MODE::LINKED_LIST;
            break;
        default:
            break;
        }

        channels[channel].dma_chop_size = (val >> 16) & 0x07;
        channels[channel].cpu_chop_size = (val >> 20) & 0x07;

        if(channel_active(channel))
        {
            // FIXME: What in the fuck is this...
            if((dpcr & 0x08000000) || (dpcr & 0x00800000) || (dpcr & 0x00080000) || (dpcr & 0x00008000) || (dpcr & 0x00000800) || (dpcr & 0x00000080) || (dpcr & 0x00000008))
                do_dma(channel);
        }
    }
    else
    {
        std::printf("controller_write: invalid offset, %d!\n", reg);
        exit(-1);
    }
}

void dma_controller::do_dma(int channel)
{
    if(channels[channel].syncmode == SYNC_MODE::LINKED_LIST)
        dma_list_copy(channel);
    else
        dma_block_copy(channel);
}

void dma_controller::dma_list_copy(int channel)
{
    std::uint32_t addr = channels[channel].base_address & 0x1ffffc;;

    if(channels[channel].direction == DIRECTION::TO_RAM)
    {
        std::printf("dma_list_copy: DMA direction TO_RAM no supported for linked list copy!\n");
        exit(-1);
    }

    if(channel != PORT::GPU)
    {
        std::printf("dma_list_copy: invalid channel %d for linked list transfer!\n", channel);
        exit(-1);
    }

    while(1)
    {
        std::uint32_t entry = bus::read_word(addr);
        std::uint32_t words_left = entry >> 24;

        while(words_left > 0)
        {
            addr = (addr + 4) & 0x1ffffc;
            std::uint32_t command = bus::read_word(addr);
            std::printf("GPU command 0x%08x\n", command);

            words_left--;
        }

        if((entry & 0x800000) != 0)
            break;

        addr = entry & 0x1ffffc;
    }

    transfer_done(channel);
}

void dma_controller::dma_block_copy(int channel)
{
    std::uint32_t addr;
    std::uint32_t words_left;
    int increment;

    addr = channels[channel].base_address;

    increment = channels[channel].direction == 0 ? 4 : -4;

    if(channels[channel].syncmode == SYNC_MODE::IMMEDIATE)
    {
        words_left = channels[channel].block_control & 0x0000ffff; // One block
    }
    else if(channels[channel].syncmode == SYNC_MODE::REQUEST)
    {
        std::uint16_t bs = channels[channel].block_control & 0x0000ffff;
        std::uint16_t ba = channels[channel].block_control >> 16;

        words_left = bs * ba;
    }
    else if(channels[channel].syncmode == SYNC_MODE::LINKED_LIST)
    {
        std::printf("dma_block_copy: SYNC_MODE::LINKED_LIST not supported for block copy!\n");
        exit(-1);
    }

    while(words_left != 0)
    {
        std::uint32_t value = 0;
        std::uint32_t cur_addr = addr & 0x1ffffc;;

        if(channels[channel].direction == DIRECTION::FROM_RAM)
        {
            value = bus::read_word(cur_addr);
        }
        else if(channels[channel].direction == DIRECTION::TO_RAM)
        {
            if(channel == PORT::OTC)
            {
                if(words_left == 1)
                    value = 0x00ffffff; // end of table marker
                else
                    value = (addr - 4) & 0x1fffff;
            }
        }

        bus::write_word(cur_addr, value);
        addr += increment;
        words_left--;
    }

    transfer_done(channel);
}