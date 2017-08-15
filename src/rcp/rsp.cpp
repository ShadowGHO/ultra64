#include <memory.h>
#include <rsp.h>

namespace ultra64
{
    rsp::rsp()
    {
        mmu->write_word(0x04040010, 0x3);
    }
}
