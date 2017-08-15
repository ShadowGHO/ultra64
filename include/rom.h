#ifndef _ULTRA64_ROM_H
#define _ULTRA64_ROM_H

#include <string>

#define ROM_LOWHIGH 0x80371240
#define ROM_HIGHLOW 0x12408037

namespace ultra64
{
    typedef struct ROM_header
    {
        uint16_t signature = 0;
        uint32_t pc = 0;
        uint16_t country_code = 0;

        char name[27] = {0};
    } ROM_header;

    class ROM
    {
      public:
        ROM(std::string filename);
        ~ROM();
        unsigned char *get_pointer();
        ROM_header get_header();
        size_t get_size();
      private:
        size_t sz;
        unsigned char *data;
        ROM_header header;
    };
}

#endif
