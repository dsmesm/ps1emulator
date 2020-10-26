//
// Created by Dima on 8/31/2020.
//

#ifndef PS1EMULATOR_MEMORYMAP_H
#define PS1EMULATOR_MEMORYMAP_H


#include <vector>
#include <tuple>


struct MemoryMap {

    static const std::vector<std::tuple<unsigned int, unsigned int>> deviceRanges;

    enum Devices : unsigned int {
        BIOS = 0,
        UncachedRAM = 1,
        CachedRAM = 2,
        RAM = 3,
        HardwareRegisters = 4,
        Scratchpad = 5,
        Kseg2 = 6,
        DMA = 7,
    };

    static bool isInRange(Devices device, unsigned int address);

};


#endif //PS1EMULATOR;_MEMORYMAP_H
