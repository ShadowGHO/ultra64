#include <fstream>
#include <cstring>
#include <sstream>
#include <pif_rom.h>

namespace ultra64
{
    PIF_ROM::PIF_ROM(std::string filename)
    {
        std::fstream rom_file;

        try
        {
            rom_file.open(filename.c_str(), std::fstream::in);
        }
        catch (std::ios_base::failure &e)
        {
            throw e.what();
        }

        if(!rom_file.good())
        {
            std::string message = "Couldn't open file " +  filename;
            throw message;
        }

        rom_file.seekg(0, std::ios::end);
        sz = rom_file.tellg();

        data = new unsigned char[sz];
        memset(data, 0, sz);
        rom_file.seekg(0, std::ios::beg);
        rom_file.read((char *)data, sz);
        rom_file.close();

        uint32_t temp = 0, count = 0;
        while(count < 0x7C0)
        {
            temp = (data[count + 2] << 0) | (data[count + 3] << 8) | (data[count + 0] << 16) | (data[count + 1] << 24);
            data[count + 2] = temp >> 24;
            data[count + 3] = temp >> 16;
            data[count + 0] = temp >> 8;
            data[count + 1] = temp;
            count += 4;
        }
    }

    unsigned char *PIF_ROM::get_pointer()
    {
        return data;
    }

    PIF_ROM::~PIF_ROM()
    {
        delete data;
    }
}
