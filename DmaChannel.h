//
// Created by Dima on 9/8/2020.
//

#ifndef PS1EMULATOR_DMACHANNEL_H
#define PS1EMULATOR_DMACHANNEL_H


#include "Dma.h"

struct Dma;

struct DmaChannel {
    DmaChannel(int number);

    int channelNumber;

    //channel control register
    unsigned int transferDirection = 0;
    unsigned int addressUpdateMode = 0;
    unsigned int choppingMode = 0;
    unsigned int syncMode = 0;
    unsigned int choppingDmaWindow = 0;
    unsigned int choppingCpuWindow = 0;
    unsigned int enable = 0;

    unsigned int manualTrigger = 0;

    //channel block register
    unsigned int blockSize = 0;
    unsigned int blocksCount = 0; // syncMode 1 only

    //base address register
    unsigned int baseAddress = 0;

    void setControlRegister(unsigned int value);
    unsigned int getControlRegister();

    void disableChannel();
};


#endif //PS1EMULATOR_DMACHANNEL_H
