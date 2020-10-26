#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"
//
// Created by Dima on 9/8/2020.
//

#include "Dma.h"
#include "Ram.h"
#include "Interrupts.h"
#include "HardwareRegisters.h"

void Dma::startDma(int channelNumber) {
    if (HardwareRegisters::dma.channels[channelNumber].syncMode == 0)
    {
        blockTransfer(channelNumber); // immediate/Manual
        int c = 0;
    }
    else if (HardwareRegisters::dma.channels[channelNumber].syncMode == 2)
    {
        linkedListTransfer(channelNumber);
    }
    else
    {
        requestedBlockTransfer(channelNumber);
    }
}

void Dma::linkedListTransfer(int channelNumber) {
    auto baseAddress = HardwareRegisters::dma.channels[channelNumber].baseAddress & 0x1ffffc;

    while (true)
    {
        auto header = Ram::ram[baseAddress / 4];
        auto blockCount = header >> 24;

        while (blockCount > 0)
        {
            baseAddress = (baseAddress + 4) & 0x1ffffc;


            auto command = Ram::ram[baseAddress / 4];
            if(!HardwareRegisters::gpu.gp0Opcodes.contains(command  >> 24)){
                throw std::exception();
            }
            auto tuple = HardwareRegisters::gpu.gp0Opcodes[(command & 0xff000000) >> 24];
            auto commandsToExecute = std::get<1>(tuple);
            //auto commandsToExecute = HardwareRegisters::gpu.gp0Opcodes[(command & 0xff000000) >> 24].Item2;

            if(commandsToExecute != 1)
            {
                while(commandsToExecute > 1)
                {
                    baseAddress = (baseAddress + 4) & 0x1ffffc;
                    HardwareRegisters::gpu.gpuBuffer.push(Ram::ram[baseAddress / 4]);
                    commandsToExecute--;
                    blockCount--;
                }
            }
            HardwareRegisters::gpu.parseGp0Opcode(command);

            blockCount--;
        }

        if ((header & 0x800000) != 0)
        {
            break;
        }

        baseAddress = header & 0x1ffffc;
    }

    HardwareRegisters::dma.channels[channelNumber].disableChannel();
}

void Dma::requestedBlockTransfer(int channelNumber) {
    int increment = HardwareRegisters::dma.channels[channelNumber].addressUpdateMode == 0 ? 4 : -4;
    auto blockSize = HardwareRegisters::dma.channels[channelNumber].blockSize * HardwareRegisters::dma.channels[channelNumber].blocksCount;
    auto baseAddress = HardwareRegisters::dma.channels[channelNumber].baseAddress;

    while (blockSize > 0)
    {
        auto currentAddress = baseAddress & 0x1ffffc;

        if (HardwareRegisters::dma.channels[channelNumber].transferDirection == 1) // from main RAM
        {
            unsigned int data = 0;

            if (HardwareRegisters::dma.channels[channelNumber].channelNumber == 2)
            {
                data = Ram::ram[currentAddress / 4];
                HardwareRegisters::gpu.parseGp0Opcode(data);
            }
            else
            {
                throw std::exception();
            }

        }
        else if (HardwareRegisters::dma.channels[channelNumber].transferDirection == 0)
        {
            if (HardwareRegisters::dma.channels[channelNumber].channelNumber == 2)
            {
                Ram::ram[currentAddress / 4] = HardwareRegisters::gpu.gpuReadBuffer.front();
                HardwareRegisters::gpu.gpuReadBuffer.pop();
            }
            else
            {
                throw std::exception();
            }
        }
        else
        {
            throw std::exception();
        }

        baseAddress = (unsigned int)((int)baseAddress + increment);
        blockSize--;
    }

    HardwareRegisters::dma.channels[channelNumber].disableChannel();
}

void Dma::blockTransfer(int channelNumber) {
    int increment = HardwareRegisters::dma.channels[channelNumber].addressUpdateMode == 0 ? 4 : -4;
    auto blockSize = HardwareRegisters::dma.channels[channelNumber].blockSize; // equals blockSize * blockCount for
    auto baseAddress = HardwareRegisters::dma.channels[channelNumber].baseAddress;

    while (blockSize > 0)
    {
        auto currentAddress = baseAddress & 0x1ffffc;

        if (HardwareRegisters::dma.channels[channelNumber].transferDirection == 0) // to main RAM
        {
            unsigned int data;

            if (HardwareRegisters::dma.channels[channelNumber].channelNumber == 6)
            {
                if (blockSize == 1)
                {
                    data = 0xffffff;
                }
                else
                {
                    data = (baseAddress - 4) & 0x1fffff;
                }
            }
            else
            {
                throw std::exception();
            }

            Ram::ram[currentAddress / 4] = data;
        }
        else
        {
            throw std::exception();
        }

        baseAddress = (unsigned int)((int)baseAddress + increment);
        blockSize--;
    }

    HardwareRegisters::dma.channels[channelNumber].disableChannel();
}

void Dma::setInterruptRegister(unsigned int value) {
    auto currentIrqVal = (interruptRegister & 0x7f000000) >> 24;
    auto currentMasterIrq = interruptRegister >> 31;
    auto newIrq = (value & 0x7f000000) >> 24;
    interruptRegister = 0;

    interruptRegister |= value & 0x3f; //padding
    interruptRegister |= value & 0x8000; // force irq
    interruptRegister |= value & 0x7f0000; // irq enable
    interruptRegister |= value & 0x800000; //master irq

    auto channel0Irq = currentIrqVal & 0x1;
    auto channel1Irq = (currentIrqVal >> 1) & 0x1;
    auto channel2Irq = (currentIrqVal >> 2) & 0x1;
    auto channel3Irq = (currentIrqVal >> 3) & 0x1;
    auto channel4Irq = (currentIrqVal >> 4) & 0x1;
    auto channel5Irq = (currentIrqVal >> 5) & 0x1;
    auto channel6Irq = (currentIrqVal >> 6) & 0x1;

    unsigned int interruptAck = 0;
    interruptAck |= channel0Irq & (newIrq & 0x1); //dma0 irq
    interruptAck |= (channel1Irq & ((newIrq >> 1) & 0x1)) << 1; //dma1 irq
    interruptAck |= (channel2Irq & ((newIrq >> 2) & 0x1)) << 2; //dma2 irq
    interruptAck |= (channel3Irq & ((newIrq >> 3) & 0x1)) << 3; //dma3 irq
    interruptAck |= (channel4Irq & ((newIrq >> 4) & 0x1)) << 4; //dma4 irq
    interruptAck |= (channel5Irq & ((newIrq >> 5) & 0x1)) << 5; //dma5 irq
    interruptAck |= (channel6Irq & ((newIrq >> 6) & 0x1)) << 6; //dma6 irq
    interruptRegister |= interruptAck << 24;
//8650752
    /*interruptRegister &= (~value) & 0x1000000;
    interruptRegister &= (~value) & 0x2000000;
    interruptRegister &= (~value) & 0x4000000;
    interruptRegister &= (~value) & 0x8000000;
    interruptRegister &= (~value) & 0x10000000;
    interruptRegister &= (~value) & 0x20000000;
    interruptRegister &= (~value) & 0x40000000;*/

    if((((interruptRegister >> 15) & 0x1) != 0) || ((interruptRegister >> 23 != 0) && (((interruptRegister & 0x7f0000) & (interruptRegister & 0x7f000000)) > 0))){
        interruptRegister |= (1 << 31);
        if(1 - currentMasterIrq == 1){
            Interrupts::dma = 1;
        }
    }else{
        interruptRegister |= (0 << 31);
    }
}

Dma::Dma() {
    channels ={
        DmaChannel(0),
        DmaChannel(1),
        DmaChannel(2),
        DmaChannel(3),
        DmaChannel(4),
        DmaChannel(5),
        DmaChannel(6),
    };
}


#pragma clang diagnostic pop