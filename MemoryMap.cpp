//
// Created by Dima on 8/31/2020.
//

#include "MemoryMap.h"

const std::vector<std::tuple<unsigned int, unsigned int>> MemoryMap::deviceRanges = {
        std::tuple<unsigned int, unsigned int>(0xbfc00000, 0xbfc7ffff), // BIOS
        std::tuple<unsigned int, unsigned int>(0xa0000000, 0xa01fffff), // uncached RAM mirror
        std::tuple<unsigned int, unsigned int>(0x80000000, 0x801fffff), // cached RAM mirror
        std::tuple<unsigned int, unsigned int>(0x00000000, 0x001fffff), // main RAM
        std::tuple<unsigned int, unsigned int>(0x1f801000, 0x1f802fff), // hardware registers
        std::tuple<unsigned int, unsigned int>(0x1f800000, 0x1f801000 - 1), // scratchpad memory
        std::tuple<unsigned int, unsigned int>(0xfffe0000, 0xfffe01ff), // kseg2
        std::tuple<unsigned int, unsigned int>(0x1f801080, 0x1f8010ff), // DMA
};

bool MemoryMap::isInRange(MemoryMap::Devices device, unsigned int address) {
    auto* _device = &deviceRanges[device];
    return ((address >= std::get<0>(*_device)) && (address <= std::get<1>(*_device)));
}
