#include <memory.h>
#include <rsp.h>

namespace ultra64
{
    rsp::rsp()
    {
        mmu->write_word(SP_STATUS_REG, SP_STATUS_HALT);
    }
}
