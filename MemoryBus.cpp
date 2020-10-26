//
// Created by Dima on 9/2/2020.
//

#include "MemoryBus.h"
#include "MemoryMap.h"
#include <iostream>

Bios MemoryBus::bios = Bios();
HardwareRegisters MemoryBus::hardwareRegisters = HardwareRegisters();
Ram MemoryBus::ram = Ram();

unsigned int MemoryBus::read32(unsigned int address) {
    if (MemoryMap::isInRange(MemoryMap::BIOS, address)){
        return bios.read32(address);
    } else if (MemoryMap::isInRange(MemoryMap::UncachedRAM, address)){
        return ram.read32(address - std::get<0>(MemoryMap::deviceRanges[MemoryMap::UncachedRAM]));
    }else if (MemoryMap::isInRange(MemoryMap::CachedRAM, address)) {
        //std::cout << "Cached RAM access" << std::endl;
        return ram.read32(address - std::get<0>(MemoryMap::deviceRanges[MemoryMap::CachedRAM]));
    }else if(MemoryMap::isInRange(MemoryMap::RAM, address)){
        return ram.read32(address - std::get<0>(MemoryMap::deviceRanges[MemoryMap::RAM]));
    }else if(MemoryMap::isInRange(MemoryMap::HardwareRegisters, address)){
        return hardwareRegisters.read32(address);
    }else{
        throw std::exception();
    }
}

void MemoryBus::write32(unsigned int address, unsigned int value) {
    if(MemoryMap::isInRange(MemoryMap::HardwareRegisters, address)){
        hardwareRegisters.write32(address, value);
    }else if(MemoryMap::isInRange(MemoryMap::UncachedRAM, address)){
        ram.write32(address - std::get<0>(MemoryMap::deviceRanges[MemoryMap::UncachedRAM]), value);
    }else if(MemoryMap::isInRange(MemoryMap::RAM, address)){
        ram.write32(address - std::get<0>(MemoryMap::deviceRanges[MemoryMap::RAM]), value);
    }else if(address == 0xfffe0130){ // cache control register
        std::cout << "Cache control register accessed" << std::endl;
    }else if(MemoryMap::isInRange(MemoryMap::CachedRAM, address)) {
        //std::cout << "Cached RAM access" << std::endl;
        ram.write32(address - std::get<0>(MemoryMap::deviceRanges[MemoryMap::CachedRAM]), value);
    }else{
        throw std::exception();
    }
}

unsigned int MemoryBus::read16(unsigned int address) {
    bool highBits = (address % 4 == 0);

    if (MemoryMap::isInRange(MemoryMap::HardwareRegisters, address)){
        return hardwareRegisters.read16(address, highBits);
    } else if(MemoryMap::isInRange(MemoryMap::CachedRAM, address)){
        return ram.read16(address - std::get<0>(MemoryMap::deviceRanges[MemoryMap::CachedRAM]), highBits);
    } else {
        throw std::exception();
    }
}

void MemoryBus::write16(unsigned int address, unsigned int value) {
    bool highBits = (address % 4 == 0);

    if(MemoryMap::isInRange(MemoryMap::HardwareRegisters, address)){
        hardwareRegisters.write16(address, value, highBits);
    }else if(MemoryMap::isInRange(MemoryMap::RAM, address)){
        ram.write16(address - std::get<0>(MemoryMap::deviceRanges[MemoryMap::RAM]), value, highBits);
    }else if(MemoryMap::isInRange(MemoryMap::CachedRAM, address)){
        ram.write16(address - std::get<0>(MemoryMap::deviceRanges[MemoryMap::CachedRAM]), value, highBits);
    }else{
        throw std::exception();
    }
}

void MemoryBus::write8(unsigned int address, unsigned int value) {
    auto offset = address % 4;

    if(MemoryMap::isInRange(MemoryMap::HardwareRegisters, address)){
        hardwareRegisters.write8(address, value, offset);
    }else if(MemoryMap::isInRange(MemoryMap::CachedRAM, address)){
        //std::cout << "Cached RAM access" << std::endl;
        ram.write8(address - std::get<0>(MemoryMap::deviceRanges[MemoryMap::CachedRAM]), value, offset);
    }else if(MemoryMap::isInRange(MemoryMap::RAM, address)){
        ram.write8(address - std::get<0>(MemoryMap::deviceRanges[MemoryMap::RAM]), value, offset);
    }else if(MemoryMap::isInRange(MemoryMap::UncachedRAM, address)){
        ram.write8(address - std::get<0>(MemoryMap::deviceRanges[MemoryMap::UncachedRAM]), value, offset);
    }else{
        throw std::exception();
    }
}

unsigned int MemoryBus::read8(unsigned int address) {
    auto offset = address % 4;

    if (MemoryMap::isInRange(MemoryMap::BIOS, address)){
        return bios.read8(address);
    }else if (address >= 0x1f000084 && address <= 0x1f000084 + 0x2c - 1){ // expansion 1
        std::cout << "Expansion 1 access" << std::endl;
        return 0xff;
    }else if (MemoryMap::isInRange(MemoryMap::CachedRAM, address)){
        //std::cout << "Cached RAM access" << std::endl;
        return ram.read8(address - std::get<0>(MemoryMap::deviceRanges[MemoryMap::CachedRAM]), offset);
    }else if (MemoryMap::isInRange(MemoryMap::RAM, address)){
        return ram.read8(address - std::get<0>(MemoryMap::deviceRanges[MemoryMap::RAM]), offset);
    }else if(MemoryMap::isInRange(MemoryMap::HardwareRegisters, address)){
        return hardwareRegisters.read8(address, offset);
    }else{
        throw std::exception();
    }
}
