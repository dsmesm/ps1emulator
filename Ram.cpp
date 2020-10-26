//
// Created by Dima on 9/4/2020.
//

#include "Ram.h"

std::vector<unsigned int> Ram::ram = std::vector<unsigned int>(524288);

unsigned int Ram::read32(unsigned int address) {
    return ram[address / 4];
}

void Ram::write32(unsigned int address, unsigned int value) {
    ram[address / 4] = value;
}

void Ram::write16(unsigned int address, unsigned int value, bool highBits) {
    address /= 4;

    if (!highBits){
        ram[address] = ram[address] & (unsigned int)0x0000ffff;
        ram[address] += value << (unsigned int)16;
    }else{
        ram[address] = ram[address] & (unsigned int)0xffff0000;
        ram[address] += (unsigned short)value;
    }
}

void Ram::write8(unsigned int address, unsigned int value, unsigned int offset) {
    address /= 4;

    if (offset == 3){
        ram[address] = ram[address] & (unsigned int)0x00ffffff;
        ram[address] += value << (unsigned int)24;
    }else if (offset == 2){
        ram[address] = ram[address] & 0xff00ffff;
        ram[address] += value << (unsigned int)16;
    }else if (offset == 1){
        ram[address] = ram[address] & 0xffff00ff;
        ram[address] += value << (unsigned int)8;
    }else{
        ram[address] = ram[address] & 0xffffff00;
        ram[address] += (unsigned char)value;
    }
}

unsigned int Ram::read8(unsigned int address, unsigned int offset) {
    if (offset == 3){
        return ram[address / 4] >> (unsigned int)24;
    }else if (offset == 2){
        return (ram[address / 4] & (unsigned int)0x00ff0000) >> (unsigned int)16;
    }else if (offset == 1){
        return (ram[address / 4] & (unsigned int)0x0000ff00) >> (unsigned int)8;
    }else{
        return (ram[address / 4] & (unsigned int)0x000000ff);
    }
}

unsigned int Ram::read16(unsigned int address, bool highBits) {
    if (!highBits) {
        auto value = ram[address / 4] & 0xffff0000;
        return value >> 16;
    } else {
        auto value = ram[address / 4] & 0x0000ffff;
        return (unsigned short)value;
    }
}


