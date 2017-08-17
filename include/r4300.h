#ifndef _ULTRA64_R4300_H
#define _ULTRA64_R4300_h

#include <cstdint>
#include <unordered_map>

#include <memory.h>
#include <opcodes.h>

namespace ultra64
{
    class opcode_t
    {
      private:
        void decode_instruction();
      public:
        opcode_t(uint32_t instruction);
        std::string to_string();
        std::string special_to_string();
        std::string regimm_to_string();
        std::string cp0_to_string();
        uint32_t instruction = 0;
        uint8_t opcode = 0;
        uint8_t base = 0;
        uint8_t rs = 0;
        uint8_t rt = 0;
        uint8_t rd = 0;
        uint8_t sa = 0;
        uint8_t function = 0;
        uint16_t offset = 0;
    };

    class r4300
    {
      public:
        r4300();
        ~r4300();
        void step();
        opcode_t *get_instruction(uint32_t addr);
        uint32_t get_PC();
        uint64_t get_GPR(uint8_t reg);
        uint64_t get_CP0(uint8_t reg);
      private:
        void (*opcode[0x40])(r4300 *, opcode_t *);
        void (*opcode_cp0[0x40])(r4300 *, opcode_t *);
        void (*opcode_special[0x40])(r4300 *, opcode_t *);
        std::unordered_map<uint32_t, opcode_t *> op_cache;
        uint32_t PC = 0;
        uint64_t GPR[32] = { 0 };
        uint64_t CP0[32] = { 0 };

        static void not_implemented(r4300 *, opcode_t *);
        static void cp0_not_implemented(r4300 *, opcode_t *);
        static void special_not_implemented(r4300 *, opcode_t *);

        static void beql(r4300 *, opcode_t *);
        static void bnel(r4300 *, opcode_t *);
        static void bne(r4300 *, opcode_t *);
        static void lw(r4300 *, opcode_t *);
        static void sw(r4300 *, opcode_t *);
        static void andi(r4300 *, opcode_t *);
        static void addiu(r4300 *, opcode_t *);
        static void ori(r4300 *, opcode_t *);
        static void lui(r4300 *, opcode_t *);
        static void cp0(r4300 *, opcode_t *);
        static void special(r4300 *cpu, opcode_t *op);
        static void move_cp0(r4300 *, opcode_t *);
        static void jr(r4300 *, opcode_t *);
    };
}


#endif
