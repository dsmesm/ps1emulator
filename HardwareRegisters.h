//
// Created by Dima on 9/2/2020.
//

#ifndef PS1EMULATOR_HARDWAREREGISTERS_H
#define PS1EMULATOR_HARDWAREREGISTERS_H


#include "Timer.h"
#include "Dma.h"
#include "Gpu.h"
#include "CdRom.h"
#include "Controller.h"

struct HardwareRegisters {
    bool buffering = false;
    unsigned int gpuRemainingWords = 0;

    static Timer timer0;
    static Timer timer1;
    static Timer timer2;

    static Dma dma;

    static Gpu gpu;

    static CdRom cdrom;

    static Controller controller;

    void write32(unsigned int address, unsigned int value);
    unsigned int read32(unsigned int address);

    void write16(unsigned int address, unsigned int value, bool highBits);
    unsigned int read16(unsigned int address, bool highBits);

    void write8(unsigned int address, unsigned int value, unsigned int offset);
    unsigned int read8(unsigned address, unsigned int offset);
};


#endif //PS1EMULATOR_HARDWAREREGISTERS_H
