//
// Created by Dima on 8/31/2020.
//

#ifndef PS1EMULATOR_CPU_H
#define PS1EMULATOR_CPU_H


#include <vector>
#include <map>
#include <unordered_map>
#include "MemoryBus.h"
#include "Coprocessor.h"

typedef unsigned int Register;
typedef unsigned int coprocessor;

struct Cpu {


    static unsigned int cycles;

    unsigned int cpuClock = 1;
    double hblankCount = 1;
    double hblankCount2 = 1;
    double vblankCount = 1;

    bool lineCounted = false;

    static unsigned int programCounter;
    std::vector<Register> registers;
    std::vector<Coprocessor> coprocessors;

    std::unordered_map<unsigned int, void(Cpu::*)(unsigned int)> opcodeMap;
    std::unordered_map<unsigned int, void(Cpu::*)(unsigned int)> specialOpcodeMap;

    Cpu();

    unsigned int read32(unsigned int address);
    unsigned int read16(unsigned int address);
    unsigned int read8(unsigned int address);

    void write32(unsigned int address, unsigned int value);
    void write16(unsigned int address, unsigned int value);
    void write8(unsigned int address, unsigned int value);

    std::tuple<Register, Register, unsigned int> parseData(unsigned int data);
    void parseOpcode(unsigned int data);

    void loadUpperImmediate(unsigned int instruction);
    void bitwiseOrImmediate(unsigned int instruction);
    void storeWord(unsigned int instruction);
    void storeHalfword(unsigned int instruction);
    void storeByte(unsigned int instruction);
    void loadWord(unsigned int instruction);
    void loadHalfwordUnsigned(unsigned int instruction);
    void loadHalfword(unsigned int instruction);
    void loadByte(unsigned int instruction);
    void loadByteUnsigned(unsigned int instruction);
    void shiftWordLeftLogical(unsigned int instruction);
    void shiftWordLeftLogicalVariable(unsigned int instruction);
    void shiftWordRightLogicalVariable(unsigned int instruction);
    void shiftWordRightArithmetic(unsigned int instruction);
    void shiftWordRightArithmeticVariable(unsigned int instruction);
    void shiftWordRightLogical(unsigned int instruction);
    void addImmediateUnsignedWord(unsigned int instruction);
    void addImmediateWord(unsigned int instruction);
    void addUnsignedWord(unsigned int instruction);
    void add(unsigned int instruction);
    void jump(unsigned int instruction);
    void jumpAndLink(unsigned int instruction);
    void jumpRegister(unsigned int instruction);
    void jumpAndLinkRegister(unsigned int instruction);
    void _or(unsigned int instruction);
    void moveToCoprocessor(coprocessor cop, unsigned int rt, unsigned int rd);
    void moveFromCoprocessor(coprocessor cop, unsigned int rt, unsigned int rd);
    void branchOnNotEqual(unsigned int instruction);
    void branchOnEqual(unsigned int instruction);
    void branchOnGreaterThanZero(unsigned int instruction);
    void branchOnLessThanOrEqualToZero(unsigned int instruction);
    void branchOnGreaterThanOrEqualToZero(std::tuple<Register, Register, unsigned int> items);
    void branchOnGreaterThanOrEqualToZeroAndLink(std::tuple<Register, Register, unsigned int> items);
    void branchOnLessThanZero(std::tuple<Register, Register, unsigned int> items);
    void branchOnLessThanZeroAndLink(std::tuple<Register, Register, unsigned int> items);
    void setOnLessThanUnsigned(unsigned int instruction);
    void setOnLessThanImmediate(unsigned int instruction);
    void setOnLessThanImmediateUnsigned(unsigned int instruction);
    void setOnLessThan(unsigned int instruction);
    void andImmediate(unsigned int instruction);
    void _and(unsigned int instruction);
    void subtractUnsignedWord(unsigned int instruction);
    void subtractWord(unsigned int instruction);
    void divideWord(unsigned int instruction);
    void divideWordUnsigned(unsigned int instruction);
    void moveFromLo(unsigned int instruction);
    void moveToLo(unsigned int instruction);
    void moveFromHi(unsigned int instruction);
    void moveToHi(unsigned int instruction);
    void syscall(unsigned int instruction);
    void _break(unsigned int instruction);
    void returnFromException();
    void nor(unsigned int instruction);
    void multiplyUnsignedWord(unsigned int instruction);
    void multipleWord(unsigned int instruction);
    void _xor(unsigned int instruction);
    void loadWordLeft(unsigned int instruction);
    void loadWordRight(unsigned int instruction);
    void storeWordLeft(unsigned int instruction);
    void storeWordRight(unsigned int instruction);
    void coprocessorInstruction(unsigned int instruction);
    void branchInstruction(unsigned int instruction);
    void moveControlToCoprocessor(coprocessor cop, unsigned int rt, unsigned int rd);
    void xorImmediate(unsigned int instruction);

    void setRegister(Register reg, unsigned int value);

    private:
        unsigned int LO;
        unsigned int HI;

        MemoryBus bus;
};


#endif //PS1EMULATOR_CPU_H
