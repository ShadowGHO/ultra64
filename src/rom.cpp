#include <fstream>
#include <cstring>
#include <sstream>
#include <rom.h>

namespace ultra64
{
    ROM::ROM(std::string filename)
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

        uint32_t validation_check;
        memcpy(&validation_check, data, 4);

        uint32_t temp;
        size_t count = 0;

        switch(validation_check)
        {
            case ROM_LOWHIGH:
                break;
            case ROM_HIGHLOW:
                count = 0;
                while(count < sz)
                {
                    temp = (data[count + 2] << 0) | (data[count + 3] << 8) | (data[count + 0] << 16) | (data[count + 1] << 24);
                    data[count] = temp >> 24;
                    data[count + 1] = temp >> 16;
                    data[count + 2] = temp >> 8;
                    data[count + 3] = temp;
                    count += 4;
                }
                break;
            default:
                std::stringstream ss;
                ss << "Unknown signature: 0x" << std::hex << validation_check;
                std::string error = ss.str();
                throw error;
                break;
        }

        memset(&header.name, 0, 20);
        memcpy(&header.signature, data, 2);
        memcpy(&header.pc, data + 0x08, 4);
        memcpy(&header.name, data + 0x20, 20);
        memcpy(&header.country_code, data + 0x3E, 2);
    }

    unsigned char *ROM::get_pointer()
    {
        return data;
    }

    ROM_header ROM::get_header()
    {
        return header;
    }

    size_t ROM::get_size()
    {
        return sz;
    }

    ROM::~ROM()
    {
        delete data;
    }
}
