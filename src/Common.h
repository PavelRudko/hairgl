#ifndef HAIRGL_COMMON_H
#define HAIRGL_COMMON_H

#include <stdint.h>
#include <hairgl/HairTypes.h>
#include <string>

namespace HairGL
{
    class HairAsset
    {
    public:
        uint32_t restPositionsBufferID;
        uint32_t tangentsDistancesBufferID;
        uint32_t hairIndicesBufferID;
		uint32_t refVectorsBufferID;
		uint32_t globalRotationsBufferID;
		uint32_t debugBufferID;
        uint32_t segmentsCount;
        uint32_t guidesCount;
        uint32_t trianglesCount;
    };

    class HairInstance
    {
    public:
        const HairAsset* asset;
        HairInstanceSettings settings;
        uint32_t positionsBufferID;
        uint32_t previousPositionsBufferID;
    };

    std::string LoadFile(const char* path);
}

#endif