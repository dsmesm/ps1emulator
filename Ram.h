//
// Created by Dima on 9/4/2020.
//

#ifndef PS1EMULATOR_RAM_H
#define PS1EMULATOR_RAM_H


#include <vector>

struct Ram {
    static std::vector<unsigned int> ram;

    unsigned int read32(unsigned int address);
    void write32(unsigned int address, unsigned int value);

    void write16(unsigned int address, unsigned int value, bool highBits);
    unsigned int read16(unsigned int address, bool highBits);

    void write8(unsigned int address, unsigned int value, unsigned int offset);
    unsigned int read8(unsigned int address, unsigned int offset);
};


#endif //PS1EMULATOR_RAM_H
