//
// Created by Dima on 8/31/2020.
//

#include "Bios.h"
#include <fstream>
#include <iterator>
#include "MemoryMap.h"

Bios::Bios() {
    std::ifstream file(R"(C:\Users\Dima\source\repos\PS1emulator\PS1emulator\SCPH1001.BIN)", std::ifstream::binary);
    file.seekg(0, std::ifstream::end);
    int length = file.tellg();
    file.seekg(0, std::ifstream::beg);

    biosData = std::vector<std::byte>(length);
    file.read(reinterpret_cast<char*>(biosData.data()), length);

    biosData.resize(file.gcount());

    file.close();
}

unsigned int Bios::read32(unsigned int address) {
    address -= std::get<0>(MemoryMap::deviceRanges[MemoryMap::Devices::BIOS]);
    return (((unsigned int)biosData[address + 3] << 24) | ((unsigned int)biosData[address + 2] << 16) | ((unsigned int)biosData[address + 1] << 8) | ((unsigned int)biosData[address]));
}

unsigned int Bios::read8(unsigned int address) {
    address -= std::get<0>(MemoryMap::deviceRanges[MemoryMap::Devices::BIOS]);
    return (unsigned int)biosData[address];
}
