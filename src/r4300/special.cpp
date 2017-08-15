#include <r4300.h>

namespace ultra64
{
    void r4300::special(r4300 *cpu, opcode_t *op)
    {
        cpu->opcode_special[op->function](cpu, op);
    }

    void r4300::jr(r4300 *cpu, opcode_t *op)
    {
        uint32_t branch = cpu->GPR[op->rs];
        cpu->PC += 4;
        cpu->step();
        cpu->PC = branch;
    }
}
