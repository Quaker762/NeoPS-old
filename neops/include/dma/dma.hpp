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
#ifndef DMA_HPP_INCLUDED
#define DMA_HPP_INCLUDED

#include <cstdint>

#define DMA_CHANNEL0_BASE   0x1f801080 // MDECin
#define DMA_CHANNEL1_BASE   0x1f801090 // MDECout
#define DMA_CHANNEL2_BASE   0x1f8010a0 // GPU (lists + image data)
#define DMA_CHANNEL3_BASE   0x1f8010b0 // CDROM
#define DMA_CHANNEL4_BASE   0x1f8010c0 // SPU
#define DMA_CHANNEL5_BASE   0x1f8010d0 // PIO
#define DMA_CHANNEL6_BASE   0x1f8010e0 // GPU OTC

#define DMA_CTRL_REG        0x1f8010f0
#define DMA_INTERRUPT_REG   0x1f8010f4
#define DMA_1F8010F8H       0x7ffac68b
#define DMA_1F8010FCh       0x00fffff7

namespace bus
{
    enum DIRECTION
    {
        FROM_RAM = 0,
        TO_RAM,
    };

    enum ADDRESS_MODE
    {
        INCREMENT = 0,
        DECREMENT,
    };

    enum SYNC_MODE
    {
        IMMEDIATE = 0,
        REQUEST,
        LINKED_LIST,
    };

    enum PORT
    {
        MDECIN = 0,
        MDECOUT,
        GPU,
        CDROM,
        SPU,
        PIO,
        OTC,
    };

    /**
     *  DMA channel structure.
     */
    struct channel
    {
        std::uint32_t base_address;     /**< Channel base control register */
        std::uint32_t block_control;    /**< Channel block control register */
        std::uint32_t channel_control;  /**< Channel channel control register */

        DIRECTION       direction;      /**< Operation direction */
        SYNC_MODE       syncmode;       /**< Channel Synchronization mode */
        ADDRESS_MODE    addr_mode;      /**< Channel address mode (increment [+4] or decrement (-4)) */
        std::uint8_t    dma_chop_size;  /**< DMA Chopping Window Size */
        std::uint8_t    cpu_chop_size;  /**< CPU Copping Window Size */

    };

    /**
     *  The PSXs DMA controller. Allows us to transfer blocks of memory to different
     *  devices and peripherals independent of the CPU. That is, we can copy the data
     *  from main memory to
     */
    class dma_controller
    {
    public:
        dma_controller();
        ~dma_controller();

        void write_dpcr(std::uint32_t val);
        std::uint32_t read_dpcr();

        void write_dicr(std::uint32_t val);
        std::uint32_t read_dicr();

        void update_irq_active();

        void controller_write(std::uint32_t address, std::uint32_t val);
        std::uint32_t controller_read(std::uint32_t address);

        void dma_run(int channel);

        bool channel_enabled(int channel) const
        {
            return ((channels[channel].channel_control >> 24) & 1);
        }

        bool channel_trigger(int channel) const
        {
            if(channels[channel].syncmode == SYNC_MODE::IMMEDIATE)
                return ((channels[channel].channel_control >> 28) & 1);
            else
                return true;
        }

        bool channel_active(int channel)
        {
            return channel_trigger(channel) && channel_enabled(channel);
        }

    private:
        std::uint32_t dpcr;
        std::uint32_t dicr;

        channel channels[7];    /**< Our DMA channels */

        void dma_block_copy(int channel);
        void dma_list_copy(int channel);

        void do_dma(int channel);

        void transfer_done(int channel)
        {
            std::uint32_t flag = 1 << (channel + 24);
            std::uint32_t mask = 1 << (channel + 16);

            if(dicr & mask)
                dicr |= flag;

            channels[channel].channel_control &= ~0x01000000;

            update_irq_active();
        }
    };
}

#endif // DMA_HPP_INCLUDED
