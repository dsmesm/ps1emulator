//
// Created by Dima on 9/8/2020.
//

#ifndef PS1EMULATOR_DMA_H
#define PS1EMULATOR_DMA_H


#include <vector>
#include "DmaChannel.h"

struct DmaChannel;

struct Dma {
    Dma();

    unsigned int controlRegister = 0x07654321;
    unsigned int interruptRegister = 0;

    std::vector<DmaChannel> channels;

    void startDma(int channelNumber);
    void linkedListTransfer(int channelNumber);
    void requestedBlockTransfer(int channelNumber);
    void blockTransfer(int channelNumber);

    void setInterruptRegister(unsigned int value);
};


#endif //PS1EMULATOR_DMA_H
