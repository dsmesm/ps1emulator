//
// Created by Dima on 9/2/2020.
//

#include <exception>
#include <iostream>
#include "HardwareRegisters.h"
#include "Interrupts.h"
#include "MemoryMap.h"
#include "Cpu.h"

Timer HardwareRegisters::timer0 = Timer();
Timer HardwareRegisters::timer1 = Timer();
Timer HardwareRegisters::timer2 = Timer();

Dma HardwareRegisters::dma = Dma();

Gpu HardwareRegisters::gpu = Gpu();

CdRom HardwareRegisters::cdrom = CdRom();

Controller HardwareRegisters::controller = Controller();

void HardwareRegisters::write32(unsigned int address, unsigned int value) {
    if (address >= 0x1f801000 && address <= 0x1f801020) // memory control registers
    {

    } else if (address == 0x1f801060) { // RAM_SIZE register

    } else if (address == 0x1f801070){
        Interrupts::setInterruptStatus(value);
    } else if (address == 0x1f801074){
        Interrupts::setInterruptMask(value);
    } else if (MemoryMap::isInRange(MemoryMap::DMA, address)){
        if (address == 0x1f8010f0) {
            dma.controlRegister = value;
        } else if (address == 0x1f8010f4) {
            //throw std::exception();
            dma.setInterruptRegister(value);
            //dma.interruptRegister = value;
        } else if ((address & 0xfffffff0) == 0x1f8010e0) // DMA channel 6
        {
            switch (address) {
                case 0x1f8010e0:
                    dma.channels[6].baseAddress = value & 0x00ffffff;
                    break;
                case 0x1f8010e4:
                    dma.channels[6].blockSize = value & 0xffff;
                    dma.channels[6].blocksCount = value >> 16;
                    break;
                case 0x1f8010e8:
                    dma.channels[6].setControlRegister(value);
                    break;
            }
        } else if ((address & 0xfffffff0) == 0x1f8010a0) // DMA channel 2
        {
            switch (address) {
                case 0x1f8010a0:
                    dma.channels[2].baseAddress = value & 0x00ffffff;
                    break;
                case 0x1f8010a4:
                    dma.channels[2].blockSize = value & 0xffff;
                    dma.channels[2].blocksCount = value >> 16;
                    break;
                case 0x1f8010a8:
                    dma.channels[2].setControlRegister(value);
                    break;
            }
        } else {
            throw std::exception();
        }
    } else if (address == 0x1f801810) {
        if (gpu.loadingImage)
        {
            gpu.parseGp0Opcode(value);
        }
        else
        {
            if (!buffering)
            {
                if(!gpu.gp0Opcodes.contains(value  >> 24)){
                    throw std::exception();
                }

                auto tuple = gpu.gp0Opcodes[value  >> 24];
                auto commandsToExecute = std::get<1>(tuple);
                gpuRemainingWords = commandsToExecute;
            }
            if (gpuRemainingWords > 0)
            {
                gpu.gpuBuffer.push(value);
                gpuRemainingWords--;
                buffering = true;
            }
            if (gpuRemainingWords == 0)
            {
                if(Cpu::programCounter==2147813108){
                    int c = 0;
                }

                buffering = false;
                auto opcode = gpu.gpuBuffer.front();
                gpu.gpuBuffer.pop();
                if(!gpu.gp0Opcodes.contains(value  >> 24)){
                    throw std::exception();
                }
                gpu.parseGp0Opcode(opcode);
            }
        }
    } else if (address >= 0x1f801100 && address <= 0x1f801130) { // Timers registers
        if (address == 0x1f801100){
            timer0.currentValue = value & 0xffff; // timer0 - Dotclock
        } else if (address == 0x1f801104) {
            timer0.setMode(value);
        }else if (address == 0x1f801108) {
            timer0.targetValue = value & 0xffff;
        } else if (address == 0x1f801110) {
            timer1.currentValue = value & 0xffff; // timer1 - Horizontal Retrace
        } else if (address == 0x1f801114) {
            timer1.setMode(value);
        } else if (address == 0x1f801118) {
            timer1.targetValue = value & 0xffff;
        } else if (address == 0x1f801120) {
            timer2.currentValue = value & 0xffff; // timer2 - 1/8 system clock
        } else if (address == 0x1f801124) {
            timer2.setMode(value);
        } else if (address == 0x1f801128) {
            timer2.targetValue = value & 0xffff;
        } else {
            throw std::exception();
        }
    } else if (address == 0x1f801814){
        gpu.parseGp1Opcode(value);
    } else {
        throw std::exception();
    }
}

void HardwareRegisters::write16(unsigned int address, unsigned int value, bool highBits) {
    if (address >= 0x1f801c00 && address <= 0x1f801fff) {// SPU

    }else if (address >= 0x1f801100 && address <= 0x1f801130) { // Timer(s) registers
        if (address == 0x1f801100){
            timer0.currentValue = value; // timer0 - Dotclock
        } else if (address == 0x1f801104) {
            timer0.setMode(value);
         }else if (address == 0x1f801108) {
            timer0.targetValue = value;
        } else if (address == 0x1f801110) {
            timer1.currentValue = value; // timer1 - Horizontal Retrace
        } else if (address == 0x1f801114) {
            timer1.setMode(value);
        } else if (address == 0x1f801118) {
            timer1.targetValue = value;
        } else if (address == 0x1f801120) {
            timer2.currentValue = value; // timer2 - 1/8 system clock
        } else if (address == 0x1f801124) {
            timer2.setMode(value);
        } else if (address == 0x1f801128) {
            timer2.targetValue = value;
        } else {
            throw std::exception();
        }
    } else if (address == 0x1f801074) {
        Interrupts::setInterruptMask(value);
    } else if (address == 0x1f80104a) {
        controller.setControl(value);
    } else if (address == 0x1f80104e) {
        controller.baudRateReload = value;
    }else if (address == 0x1f801048) {
        controller.setMode(value);
    }else{
        throw std::exception();
    }
}

void HardwareRegisters::write8(unsigned int address, unsigned int value, unsigned int offset) {
    if(address == 0x1f802041){
        std::cout << "Accessing POST register" << std::endl;
    }else if (address == 0x1f801800){
        cdrom.setStatusRegister(value);
    }else if (address == 0x1f801803) {
        if(cdrom.portIndex == 1){
            cdrom.setInterruptFlag(value);
        }else{
            throw std::exception();
        }
    }else if (address == 0x1f801802){
        if (cdrom.portIndex == 1) // interrupt enable register
        {
            cdrom.interruptEnable = value & 0x1f;
        }
        else if (cdrom.portIndex == 0) // parameter fifo
        {
            cdrom.parameterQueue.push(value);
            cdrom.parameterFifoEmpty = 0;
        }
        else
        {
            throw std::exception();
        }
    }else if (address == 0x1f801801){
        if (cdrom.portIndex == 0) // command register
        {
            cdrom.commandQueue.push(value);
            cdrom.executeCommand();
        }
        else
        {
            throw std::exception();
        }
    }else{
        throw std::exception();
    }
}

unsigned int HardwareRegisters::read32(unsigned int address) {
    if (address == 0x1f801074){
        return Interrupts::getInterruptMask();
    } else if (MemoryMap::isInRange(MemoryMap::DMA, address)){
        if (address == 0x1f8010f0) {
            return dma.controlRegister;
        } else if (address == 0x1f8010f4) {
            //throw std::exception();
            return dma.interruptRegister;
        }
        else if ((address & 0xfffffff0) == 0x1f8010e0) // DMA channel 6
        {
            switch (address)
            {
                case 0x1f8010e0:
                    throw std::exception();
                    //break;
                case 0x1f8010e4:
                    throw std::exception();
                    //break;
                case 0x1f8010e8:
                    return dma.channels[6].getControlRegister();
                default:
                    throw std::exception();
            }
        }
        else if ((address & 0xfffffff0) == 0x1f8010a0) // DMA channel 2
        {
            switch (address)
            {
                case 0x1f8010a0:
                    //return dma.channels[2].baseAddress;
                    throw std::exception();
                    //break;
                case 0x1f8010a4:
                    throw std::exception();
                    //break;
                case 0x1f8010a8:
                    return dma.channels[2].getControlRegister();
                default:
                    throw std::exception();
            }
        }
        else
        {
            throw std::exception();
        }
    } else if (address == 0x1f801814) { //GPUSTAT
        return gpu.getGpuStat();
    } else if (address == 0x1f801810) { //GPUREAD
        if(gpu.gpuReadBuffer.empty()){
            return 0;
        }else{
            auto response = gpu.gpuReadBuffer.front();
            gpu.gpuReadBuffer.pop();
            return response;
        }
    } else if (address >= 0x1f801100 && address <= 0x1f801130) { // Timer(s) registers
        if (address == 0x1f801110){
            return timer1.currentValue;
            //return 0;
        } else {
            throw std::exception();
        }
    } else if (address == 0x1f801070){
        return Interrupts::getInterruptStatus();
    } else {
        throw std::exception();
    }
}

unsigned int HardwareRegisters::read16(unsigned int address, bool highBits) {
    if (address >= 0x1f801c00 && address <= 0x1f801fff) { // SPU
        return 0;
    } else if (address == 0x1f801074) {
        return Interrupts::getInterruptMask();
    }else if (address >= 0x1f801100 && address <= 0x1f801130) { // Timer(s) registers
         if (address == 0x1f801120) {
            return timer2.currentValue; // timer2 - 1/8 system clock
        } else {
            throw std::exception();
        }
    } else {
        throw std::exception();
    }
}

unsigned int HardwareRegisters::read8(unsigned int address, unsigned int offset) {
    if (address == 0x1f801800){
        return cdrom.getStatusRegister();
    }else if (address == 0x1f801801){
        if (cdrom.portIndex == 1){
            if (cdrom.responseQueue.size() == 1){
                cdrom.responseFifoEmpty = 0;
            }
            auto response = cdrom.responseQueue.front() & 0xff;
            cdrom.responseQueue.pop();
            return response;
        }else{
            throw std::exception();
        }
    }else if (address == 0x1f801803){
        if (cdrom.portIndex == 1){
            return cdrom.getInterruptFlag();
        }else{
            throw std::exception();
        }
    }else{
        throw std::exception();
    }
}


