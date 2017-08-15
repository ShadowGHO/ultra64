#ifndef _ULTRA64_RSP_H
#define _ULTRA64_RSP_h

#include <cstdint>

#include <memory.h>
#include <opcodes.h>

namespace ultra64
{
    class rsp
    {
      public:
        rsp();
        ~rsp();
        void step();
    };
}

#endif
