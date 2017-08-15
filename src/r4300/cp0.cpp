#include <r4300.h>

namespace ultra64
{
    void r4300::move_cp0(r4300 *cpu, opcode_t *op)
    {
        switch(op->rs)
        {
            case MTC0: /* To */
                cpu->CP0[op->rd] = cpu->GPR[op->rt];
                cpu->PC += 4;
                return;
                break;
            case MFC0: /* From */
                cpu->GPR[op->rt] = cpu->CP0[op->rd];
                cpu->PC += 4;
                return;
                break;
        }
    }
}
