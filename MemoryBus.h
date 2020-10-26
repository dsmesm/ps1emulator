//
// Created by Dima on 9/2/2020.
//

#ifndef PS1EMULATOR_MEMORYBUS_H
#define PS1EMULATOR_MEMORYBUS_H


#include "Bios.h"
#include "HardwareRegisters.h"
#include "Ram.h"

struct MemoryBus {
    static Bios bios;
    static HardwareRegisters hardwareRegisters;
    static Ram ram;

    unsigned int read32(unsigned int address);
    void write32(unsigned int address, unsigned int value);

    unsigned int read16(unsigned int address);
    void write16(unsigned int address, unsigned int value);

    unsigned int read8(unsigned int address);
    void write8(unsigned int address, unsigned int value);
};


#endif //PS1EMULATOR_MEMORYBUS_H
