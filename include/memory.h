#ifndef _ULTRA64_MEMORY_H
#define _ULTRA64_MEMORY_H

#include <rom.h>

namespace ultra64
{
    typedef struct
    {
        uint32_t offset = 0;
        void *ptr = nullptr;
    } memory_section;

    class MMU 
    {
      public:
        MMU();
        ~MMU();
        void attach_rom(unsigned char *rom, size_t sz);

        uint8_t read_byte(uint32_t addr);
        uint16_t read_hword(uint32_t addr);
        uint32_t read_word(uint32_t addr);
      
        void write_byte(uint32_t addr, uint8_t value);
        void write_hword(uint32_t addr, uint16_t value);
        void write_word(uint32_t addr, uint32_t value);
      private:
        unsigned char *pif_rom = new unsigned char[0x7C0];
        unsigned char *pif_ram = new unsigned char[0x40];
        unsigned char *rom = nullptr;
        size_t rom_sz = 0;
        unsigned char *rdram = new unsigned char[0x400000];
        unsigned char *rdram_registers = new unsigned char[0x28];
        unsigned char *sp_dmem = new unsigned char[0x1000];
        unsigned char *sp_imem = new unsigned char[0x1000];
        unsigned char *sp_registers = new unsigned char[0x20];
        unsigned char *sp_registers2 = new unsigned char[0x8];
        unsigned char *pi_registers = new unsigned char[0x34];
        unsigned char *vi_registers = new unsigned char[0x38];
        unsigned char *ai_registers = new unsigned char[0x18];
        unsigned char *dp_command_registers = new unsigned char[0x20];
        unsigned char *dp_span_registers = new unsigned char[0x10];
        unsigned char *mi_registers = new unsigned char[0x10];
        unsigned char *ri_registers = new unsigned char[0x20];
        unsigned char *si_registers = new unsigned char[0x1C];

 
        memory_section get_section(uint32_t addr);
    };
}

extern ultra64::MMU *mmu;

#define SP_STATUS_HALT		0x00000001
#define SP_STATUS_BROKE		0x00000002
#endif
