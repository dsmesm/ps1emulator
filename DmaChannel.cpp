//
// Created by Dima on 9/8/2020.
//

#include "DmaChannel.h"
#include "Interrupts.h"
#include "HardwareRegisters.h"

void DmaChannel::setControlRegister(unsigned int value) {
    if (channelNumber != 6) {
        transferDirection = value & 0x00000001;
        addressUpdateMode = (value >> 1) & 0x00000001;
        choppingMode = (value >> 8) & 0x1;
        syncMode = (value >> 9) & 0x3;
        choppingDmaWindow = (value >> 16) & 0x3;
        choppingCpuWindow = (value >> 20) & 0x3;
        enable = (value >> 24) & 0x1;
        manualTrigger = (value >> 28) & 0x1;

        if(enable == 1){
            HardwareRegisters::dma.startDma(channelNumber);
        }
    } else {
        transferDirection = 0;
        addressUpdateMode = 1;
        choppingMode = 0;
        syncMode = 0;
        choppingDmaWindow = 0;
        choppingCpuWindow = 0;
        enable = (value >> 24) & 0x1;
        manualTrigger = (value >> 28) & 0x1;

        if(enable == 1){
            HardwareRegisters::dma.startDma(channelNumber);
        }
    }
}

unsigned int DmaChannel::getControlRegister() {
    unsigned int registerValue = 0;

    registerValue |= transferDirection;
    registerValue |= addressUpdateMode << 1;
    registerValue |= choppingMode << 8;
    registerValue |= syncMode << 9;
    registerValue |= choppingDmaWindow << 16;
    registerValue |= choppingCpuWindow << 20;
    registerValue |= enable << 24;
    registerValue |= manualTrigger << 28;

    return registerValue;
}

void DmaChannel::disableChannel() {
    auto pendingInterrupts = (((0x1 << 24) << channelNumber) & ((HardwareRegisters::dma.interruptRegister << 8) & 0x7f000000));
    HardwareRegisters::dma.interruptRegister |= pendingInterrupts;
    auto c = HardwareRegisters::dma.interruptRegister;
    if(pendingInterrupts != 0){
        Interrupts::dma = 1;
    }
    manualTrigger = 0;
    enable = 0;
}

DmaChannel::DmaChannel(int number) {
    channelNumber = number;
}

