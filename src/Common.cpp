#include "Common.h"

namespace HairGL
{
    std::string LoadFile(const char* path)
    {
        auto file = fopen(path, "rb");
        fseek(file, 0, SEEK_END);
        int size = ftell(file);
        fseek(file, 0, SEEK_SET);

        std::string str(size, '\0');
        fread(&str[0], 1, size, file);
        fclose(file);

        return str;
    }
}
