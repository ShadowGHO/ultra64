#include <memory.h>
#include <iostream>
#include <sstream>

namespace ultra64
{
    MMU::MMU()
    {
    }

    MMU::~MMU()
    {
    }

    void MMU::attach_rom(unsigned char *rom, size_t sz)
    {
        this->rom = rom;
        rom_sz = sz;
    }

    uint8_t MMU::read_byte(uint32_t addr)
    {
        uint32_t phys_addr = addr & 0x1FFFFFFF;
        memory_section s;
        try
        {
            s = get_section(phys_addr);
        }
        catch(std::string e)
        {
            throw e;
        }
         
        uint8_t *p = (uint8_t *)s.ptr;

        return p[phys_addr - s.offset];
    }

    void MMU::write_byte(uint32_t addr, uint8_t value)
    {
        uint32_t phys_addr = addr & 0x1FFFFFFF;
        memory_section s;
        try
        {
            s = get_section(phys_addr);
        }
        catch(std::string e)
        {
            throw e;
        }

        uint8_t *p = (uint8_t *)s.ptr;
        p[phys_addr - s.offset] = value;
    }

    uint16_t MMU::read_hword(uint32_t addr)
    {
        uint32_t phys_addr = addr & 0x1FFFFFFF;
        memory_section s;
        try
        {
            s = get_section(phys_addr);
        }
        catch(std::string e)
        {
            throw e;
        }

        uint32_t pointer = phys_addr - s.offset;
        if(pointer) pointer /= 2;

        uint16_t *p = (uint16_t *)s.ptr;

        return p[pointer];
    }

    void MMU::write_hword(uint32_t addr, uint16_t value)
    {
        uint32_t phys_addr = addr & 0x1FFFFFFF;
        memory_section s;
        try
        {
            s = get_section(phys_addr);
        }
        catch(std::string e)
        {
            throw e;
        }

        uint32_t pointer = phys_addr - s.offset;
        if(pointer) pointer /= 2;

        uint16_t *p = (uint16_t *)s.ptr;
        p[pointer] = value;
    }

    uint32_t MMU::read_word(uint32_t addr)
    {
        memory_section s;
        uint32_t phys_addr = addr & 0x1FFFFFFF;

        try
        {
            s = get_section(phys_addr);
        }
        catch(std::string e)
        {
            throw e;
        }

        uint32_t pointer = phys_addr - s.offset;
        if(pointer) pointer /= 4;


        uint32_t *p = (uint32_t *)s.ptr;
        uint32_t value;

        asm ("movl %1, %%eax;" 
              "bswap %%eax;"
              "movl %%eax, %0;"
             :"=r"(value)        /* output */
             :"r"(p[pointer])         /* input */
             :"%eax"         /* clobbered register */
         );

        return value;
    }

    void MMU::write_word(uint32_t addr, uint32_t value)
    {
        memory_section s;
        uint32_t phys_addr = addr & 0x1FFFFFFF;
        
        try
        {
            s = get_section(phys_addr);
        }
        catch(std::string e)
        {
            throw e;
        }

        uint32_t pointer = phys_addr - s.offset;
        if(pointer) pointer /= 4;

        uint32_t *p = (uint32_t *)s.ptr;
        asm ("movl %1, %%eax;"
              "bswap %%eax;"
              "movl %%eax, %0;"
             :"=r"(p[pointer])        /* output */
             :"r"(value)         /* input */
             :"%eax"         /* clobbered register */
         );
    }

    memory_section MMU::get_section(uint32_t addr)
    {
        memory_section section;
        std::stringstream ss;
        std::string e;

        if((addr >= 0x00000000) && (addr <=0x003FFFFF))
        {
            section.offset = 0x0;
            section.ptr = rdram;
            return section;
        }

        if((addr >= 0x03F00000) && (addr <= 0x03FFFFFF))
        {
            section.offset = 0x03F00000;
            section.ptr = rdram_registers;
            return section;
        }

        if((addr >= 0x04000000) && (addr <= 0x04000FFF))
        {
            section.offset = 0x04000000;
            section.ptr = sp_dmem;
            return section;
        }

        if((addr >= 0x04001000) && (addr <= 0x04001FFF))
        {
            section.offset = 0x04001000;
            section.ptr = sp_imem;
            return section;
        }

        if((addr >= 0x04040000) && (addr <= 0x0404001F))
        {
            section.offset = 0x04040000;
            section.ptr = sp_registers;
            return section;
        }

        if((addr >= 0x04080000) && (addr <= 0x04080007))
        {
            section.offset = 0x04080000;
            section.ptr = sp_registers2;
            return section;
        }

        if((addr >= 0x04100000) && (addr <= 0x0410001F))
        {
            section.offset = 0x04100000;
            section.ptr = dp_command_registers;
            return section;
        }

        if((addr >= 0x04200000) && (addr <= 0x0420000F))
        {
            section.offset = 0x04200000;
            section.ptr = dp_span_registers;
            return section;
        }

        if((addr >= 0x04300000) && (addr <= 0x0430000F))
        {
            section.offset = 0x04300000;
            section.ptr = mi_registers;
            return section;
        }

        if((addr >= 0x04400000) && (addr <= 0x04400037))
        {
            section.offset = 0x04400000;
            section.ptr = vi_registers;
            return section;
        }

        if((addr >= 0x04500000) && (addr <= 0x04500017))
        {
            section.offset = 0x04500000;
            section.ptr = ai_registers;
            return section;
        }

        if((addr >= 0x04600000) && (addr <= 0x04600033))
        {
            section.offset = 0x04600000;
            section.ptr = pi_registers;
            return section;
        }

        if((addr >= 0x04700000) && (addr <= 0x0440001F))
        {
            section.offset = 0x04700000;
            section.ptr = ri_registers;
            return section;
        }

        if((addr >= 0x04800000) && (addr <= 0x0480001B))
        {
            section.offset = 0x04800000;
            section.ptr = si_registers;
            return section;
        }

        if((addr >= 0x10000000) && (addr <=0x1FBFFFFF))
	{
            section.offset = 0x10000000;
            if((addr - section.offset) > rom_sz)
            {
                ss << "ROM physical address 0x" << std::hex << addr << " is out of bounds.";
                std::string e = ss.str();
                throw e;
            }
	    section.ptr = rom;
            return section;
	}

        if((addr >= 0x1FC00000) && (0x1FC007BF))
        {
            section.offset = 0x1FC00000;
            section.ptr = pif_rom;
            return section;
        }

        if((addr >= 0x1FC007C0) && (0x1FC007FF))
        {
            section.offset = 0x1FC007c0;
            section.ptr = pif_ram;
            return section;
        }

        ss << "Address 0x" << std::hex << addr << " is out of bounds.";
        e = ss.str();
        throw e;
    }
}
