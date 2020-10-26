//
// Created by Dima on 8/31/2020.
//

#ifndef PS1EMULATOR_BIOS_H
#define PS1EMULATOR_BIOS_H

#include <cstddef>
#include <vector>


struct Bios {
    public:
        Bios();

        std::vector<std::byte> biosData;
        unsigned int read32(unsigned int address);
        unsigned int read8(unsigned int address);
};


#endif //PS1EMULATOR_BIOS_H
