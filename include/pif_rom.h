#ifndef _ULTRA64_PIF_ROM_H
#define _ULTRA64_PIF_ROM_H

#include <string>

namespace ultra64 
{
    class PIF_ROM
    {
      public:
        PIF_ROM(std::string filename);
        ~PIF_ROM();
        unsigned char *get_pointer();
      private:
        size_t sz;
        unsigned char *data;
    };
}

#endif
