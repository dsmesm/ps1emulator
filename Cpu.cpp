//
// Created by Dima on 8/31/2020.
//

#include <sstream>
#include <iostream>
#include "Cpu.h"

unsigned int Cpu::programCounter = 0xbfc00000;
unsigned int Cpu::cycles = 0;

int ttyCounter = 0;

Cpu::Cpu() {

    registers = std::vector<Register>(32);
    coprocessors = std::vector<Coprocessor>(4);

    HI = 0;
    LO = 0;

    coprocessors[0] = Coprocessor(16);
    coprocessors[2] = Coprocessor(64);

    bus = MemoryBus();

    opcodeMap = {
            { 0b001111, &Cpu::loadUpperImmediate }, // lui - load upper immediate
            { 0b001101, &Cpu::bitwiseOrImmediate }, // ori - bitwise or immediate
            { 0b101011, &Cpu::storeWord }, // sw - store word
            { 0b101001, &Cpu::storeHalfword }, // sh - store halfword
            { 0b101000, &Cpu::storeByte }, // sb - store byte
            { 0b100011, &Cpu::loadWord }, // lw - load word
            { 0b100001, &Cpu::loadHalfword }, // lh - load halfword
            { 0b100101, &Cpu::loadHalfwordUnsigned }, // lhu - load halfword unsigned
            { 0b100000, &Cpu::loadByte }, // lb - load byte
            { 0b100100, &Cpu::loadByteUnsigned }, // lbu - load byte unsigned
            { 0b001001, &Cpu::addImmediateUnsignedWord }, // addiu - add immediate unsigned word
            { 0b001000, &Cpu::addImmediateWord }, // addi - add immediate word, should generate overflow exception
            { 0b000010, &Cpu::jump }, // j - jump
            { 0b000011, &Cpu::jumpAndLink }, // jal - jump and link*/
            { 0b000101, &Cpu::branchOnNotEqual },  // bne - branch on not equal
            { 0b000100, &Cpu::branchOnEqual }, // beq - branch on equal
            { 0b000111, &Cpu::branchOnGreaterThanZero }, // bgtz - branch on greater than zero
            { 0b000110, &Cpu::branchOnLessThanOrEqualToZero }, // blez - branch on less than or equal to zero
            { 0b001100, &Cpu::andImmediate }, // andi - and immediate
            { 0b001010, &Cpu::setOnLessThanImmediate }, // slti - Set On Less Than Immediate
            { 0b001011, &Cpu::setOnLessThanImmediateUnsigned }, // sltiu - Set On Less Than Immediate Unsigned
            /* may not work*/ { 0b100010, &Cpu::loadWordLeft }, //lwl - load word left
            /* may not work*/ { 0b100110, &Cpu::loadWordRight }, //lwr - load word right
            /* may not work*/ { 0b101010, &Cpu::storeWordLeft }, //swl - store word left
            /* may not work*/ { 0b101110, &Cpu::storeWordRight }, //swr - store word right*/
            { 0b001110, &Cpu::xorImmediate },
    };

    specialOpcodeMap = {
            { 0b000000, &Cpu::shiftWordLeftLogical }, // sll - shift word left logical
            { 0b100101, &Cpu::_or }, // or - or
            { 0b101011, &Cpu::setOnLessThanUnsigned }, // sltu - set on less than unsigned
            { 0b101010, &Cpu::setOnLessThan }, // slt - set on less than
            { 0b100001, &Cpu::addUnsignedWord }, // addu - add unsigned word
            { 0b001000, &Cpu::jumpRegister }, // jr - jump register
            { 0b100100, &Cpu::_and }, // and - and
            { 0b100000, &Cpu::add }, // add - add word
            { 0b001001, &Cpu::jumpAndLinkRegister }, // jalr - jump and link register
            { 0b100011, &Cpu::subtractUnsignedWord }, // subu - subtract unsigned word
            /*{ 0b_10_0010, SubtractWord }, // sub - subtract word*/
            { 0b000011, &Cpu::shiftWordRightArithmetic }, // sra - shift word right arithmetic
            { 0b000111, &Cpu::shiftWordRightArithmeticVariable }, // srav - shift word right arithmetic variable
            { 0b000010, &Cpu::shiftWordRightLogical }, // srl - shift word right logical
            { 0b000110, &Cpu::shiftWordRightLogicalVariable }, // srlv - Shift Word Right Logical Variable
            { 0b000100, &Cpu::shiftWordLeftLogicalVariable }, // sllv - Shift Word Left Logical Variable
            { 0b011010, &Cpu::divideWord }, // div - divide word
            { 0b011011, &Cpu::divideWordUnsigned }, // divu - divide word unsigned
            { 0b010010, &Cpu::moveFromLo }, // mflo - move from lo
            { 0b010011, &Cpu::moveToLo }, // mtlo - move to lo
            { 0b010000, &Cpu::moveFromHi }, // mfhi - move from hi
            { 0b010001, &Cpu::moveToHi }, // mthi - move to hi
            { 0b001100, &Cpu::syscall }, // SYSCALL
            /*{ 0b_00_1101, BREAK }, // BREAK*/
            { 0b100111, &Cpu::nor }, // nor - nor
            { 0b011001, &Cpu::multiplyUnsignedWord }, // multu - multiply unsigned word
            /*{ 0b_01_1000, MultiplyWord }, // mult - multipy word*/
            { 0b100110, &Cpu::_xor }, // xor - xor
    };

}

void Cpu::loadUpperImmediate(unsigned int instruction) {
    auto items = parseData(instruction);
    auto rt = std::get<1>(items);
    auto immediate = std::get<2>(items);

    setRegister(rt, immediate << (unsigned int)16);
}

void Cpu::bitwiseOrImmediate(unsigned int instruction) {
    auto items = parseData(instruction);
    auto rs = std::get<0>(items);
    auto rt = std::get<1>(items);
    auto immediate = std::get<2>(items);

    setRegister(rt, registers[rs] | immediate);
}

void Cpu::parseOpcode(unsigned int data) {
    Cpu::programCounter += 4;

    unsigned int instruction = data >> (unsigned int)26;

    if ((instruction & (unsigned int)0b111100) == 0b010000){ // coprocessors related operation
         coprocessorInstruction(data);
    }else if(instruction == 0b000000){
        auto specialOpcode = data & (unsigned int)0x3f;
        (this->*specialOpcodeMap[specialOpcode])(data);
    }else if(instruction == 0b000001){
        branchInstruction(data);
    }else{
        (this->*opcodeMap[instruction])(data);
    }

    //ntsc clocks are used
    switch (HardwareRegisters::timer0.clockSource) { //timer0 increment
        case 0:
        case 2:
            HardwareRegisters::timer0.incrementTimer0(hblankCount >= 2560 && hblankCount <= 3413);

            break;
        case 1:
        case 3:
            throw std::exception();
            //HardwareRegisters::timer0.incrementTimer0();
            break;
    }

    switch (HardwareRegisters::timer1.clockSource) { //timer1 increment
        case 0:
        case 2:
            HardwareRegisters::timer0.incrementTimer1(vblankCount >= 825946 && vblankCount <= 901032);


            break;
        case 1:
        case 3:
            //throw std::exception();
            if(hblankCount2 > 3413){
                hblankCount2 = 1;
                lineCounted = false;
            }
            if(hblankCount2 >= 2560 && hblankCount2 <= 3413){
                if(!lineCounted){
                    HardwareRegisters::timer1.incrementTimer1(vblankCount >= 825946 && vblankCount <= 901032);
                }
                lineCounted = true;
            }


            break;
    }

    switch (HardwareRegisters::timer2.clockSource) { //timer2 increment
        case 0:
        case 1:
            HardwareRegisters::timer2.incrementTimer2();
            break;
        case 2:
        case 3:
            if(cpuClock == 8){
                cpuClock = 1;
                HardwareRegisters::timer2.incrementTimer2();
            }

            break;
    }

    if(hblankCount > 3413){
        if(HardwareRegisters::gpu.interlaceLines == 1){
            HardwareRegisters::gpu.interlaceLines = 0;
        }else{
            HardwareRegisters::gpu.interlaceLines = 1;
        }
        hblankCount = 1;
    }
    if(hblankCount2 > 3413){
        hblankCount2 = 1;
    }
    if(cpuClock == 8){
        cpuClock = 1;
    }
    if(vblankCount > 901032){
        vblankCount = 1;
    }

    cpuClock++;
    hblankCount += 1 * 1.58;
    hblankCount2 += 1 * 1.58;
    vblankCount += 1 * 1.58;

}

std::tuple<Register, Register, unsigned int> Cpu::parseData(unsigned int data) {
    Register source = (data & (unsigned int)0x3e00000) >> (unsigned int)21;
    Register target = (data & (unsigned int)0x1f0000) >> (unsigned int)16;
    unsigned int value = data & (unsigned int)0xffff;

    return std::tuple<Register, Register, unsigned int>(source, target, value);
}

unsigned int Cpu::read32(unsigned int address) {
    if((address % 4) != 0){
        throw std::exception();
    }

    if(address >= 0xa0 && address <= 0xd0 && cycles >= 157542590){
        if(address >= 0xb0 && address < 0xc0 && registers[9] != 0x3d){
            int c = 0;
            registers[0] = 0;
        }else if(address >= 0xb0 && address < 0xc0 && registers[9] == 0x3d && address%176 == 0){
            std::cout << static_cast<char>(registers[4]);
        }
        if(address >= 0xa0 && address < 0xb0 && registers[9] != 0x3f){
            int d = 0;
            registers[0] = 0;
        }
        if(address >= 0xc0 && address < 0xd0 ){
            int e = 0;
            registers[0] = 0;
        }
        //registers[0] = 0;
        /*if(address >= 0xb0 && address < 0xc0){
            ttyCounter++;
            if(ttyCounter < 3){
                std::cout << static_cast<char>(registers[4]);
            }else{
                ttyCounter = 0;
            }
        }*/
    }

    return bus.read32(address);
}

void Cpu::write32(unsigned int address, unsigned int value) {
    if((address % 4) != 0){
        throw std::exception();
    }

    /*if(address == 2148396628){
        registers[0] = 0;
    }*/



    if(address == 2147844796 - 8 || address == 2147844796 - 2147483648 - 8 /*|| address == 2147844796 - 2684354560*/){
        registers[0] = 0;
    }

    bus.write32(address, value);
}

void Cpu::write16(unsigned int address, unsigned int value) {
    if((address % 2) != 0){
        throw std::exception();
    }

    /*if(address == 2148396628){
        registers[0] = 0;
    }*/

    if(address == 2147844796 - 8 || address == 2147844796 - 2147483648 - 8 /*|| address == 2147844796 - 2684354560*/){
        registers[0] = 0;
    }

    bus.write16(address, value);
}

unsigned int Cpu::read16(unsigned int address) {
    if((address % 2) != 0){
        throw std::exception();
    }

    if(address >= 0xa0 && address <= 0xd0 && cycles >= 157000000){
        registers[0] = 0;
    }

    return bus.read16(address);
}

unsigned int Cpu::read8(unsigned int address) {
    if(address >= 0xa0 && address <= 0xd0 && cycles >= 157000000){
        registers[0] = 0;
    }

    return bus.read8(address);
}

void Cpu::write8(unsigned int address, unsigned int value) {
    bus.write8(address, value);

    /*if(address == 2148396628){
        registers[0] = 0;
    }*/

    if(address == 2147844796 - 8 || address == 2147844796 - 2147483648 - 8 /*|| address == 2147844796 - 2684354560*/){
        registers[0] = 0;
    }
}


void Cpu::storeWord(unsigned int instruction) {
    auto items = parseData(instruction);
    auto base = std::get<0>(items);
    auto rt = std::get<1>(items);
    auto offset = std::get<2>(items);

    if((coprocessors[0].registers[12] & (unsigned int)0x10000) != 0){
        //isolated cache
    } else {
        auto signedOffset = (short)offset;
        write32(registers[base] + (unsigned int)signedOffset, registers[rt]);
    }
}

void Cpu::loadWord(unsigned int instruction) {
    auto items = parseData(instruction);
    auto base = std::get<0>(items);
    auto rt = std::get<1>(items);
    auto offset = std::get<2>(items);

    Cpu::cycles++;

    auto signedOffset = (short)offset;
    if(cycles == 158344921){
        registers[0] = 0;
    }
    setRegister(rt, read32(registers[base] + (unsigned int)signedOffset));

    parseOpcode(read32(programCounter));
}

void Cpu::loadHalfwordUnsigned(unsigned int instruction) {
    auto items = parseData(instruction);
    auto base = std::get<0>(items);
    auto rt = std::get<1>(items);
    auto offset = std::get<2>(items);

    Cpu::cycles++;

    auto signedOffset = (short)offset;
    auto value = (unsigned short)(read16(registers[base] + (unsigned int)signedOffset));
    setRegister(rt, (unsigned int)value);

    parseOpcode(read32(programCounter));
}

void Cpu::shiftWordLeftLogical(unsigned int instruction) {
    auto items = parseData(instruction);
    auto rt = std::get<1>(items);
    auto value = std::get<2>(items);
    auto rd = (value << (unsigned int)16) >> (unsigned int)27;
    auto sa = (value << (unsigned int)21) >> (unsigned int)27;

    setRegister(rd,registers[rt] << sa);
}

void Cpu::addImmediateUnsignedWord(unsigned int instruction) {
    auto items = parseData(instruction);
    auto rs = std::get<0>(items);
    auto rt = std::get<1>(items);
    auto immediate = std::get<2>(items);


    setRegister(rt, registers[rs] + (short)immediate);
    //add catch for overflow
}

void Cpu::jump(unsigned int instruction) {
    auto target = instruction & (unsigned int)0x3FFFFFF;

    Cpu::cycles++;
    parseOpcode(read32(Cpu::programCounter));
    Cpu::programCounter -= 4;
    Cpu::programCounter = (Cpu::programCounter & 0xf0000000) | (target << (unsigned int)2);
}

void Cpu::_or(unsigned int instruction) {
    auto items = parseData(instruction);
    auto rs = std::get<0>(items);
    auto rt = std::get<1>(items);
    auto rd = std::get<2>(items) >> (unsigned int)11;

    setRegister(rd, registers[rs] | registers[rt]);
}

void Cpu::coprocessorInstruction(unsigned int instruction) {
    auto items = parseData(instruction);

    if (std::get<0>(items) == 0b00100) { // mtcz - move to coprocessor
        moveToCoprocessor((instruction >> (unsigned int)26) & (unsigned int)0b000011, std::get<1>(items), std::get<2>(items) >> (unsigned int)11);
    }else if (std::get<0>(items) == 0b00000) { // mfcz - move from coprocessor
        moveFromCoprocessor((instruction >> (unsigned int)26) & (unsigned int)0b000011, std::get<1>(items), std::get<2>(items) >> (unsigned int)11);
    }
    else if (std::get<0>(items) == 0b10000) // rfe - return from exception
    {
        returnFromException();
    }
    else if (std::get<0>(items) == 0b00110){
        //move to gte control register
        moveControlToCoprocessor((instruction >> (unsigned int)26) & (unsigned int)0b000011, std::get<1>(items), std::get<2>(items) >> (unsigned int)11);
    }
    else
    {
        throw std::exception();
    }
}

void Cpu::moveToCoprocessor(coprocessor cop, unsigned int rt, unsigned int rd) {
    coprocessors[cop].registers[rd] = registers[rt];
}

void Cpu::branchOnNotEqual(unsigned int instruction) {
    auto items = parseData(instruction);
    auto rs = std::get<0>(items);
    auto rt = std::get<1>(items);
    auto offset = std::get<2>(items);

    if(instruction == 344063990 || programCounter == 2147840524){
        registers[0] = 0;
        //registers[4] = 65535;
    }

    if (registers[rs] != registers[rt])
    {
        Cpu::cycles++;
        auto currentPC = Cpu::programCounter;
        auto signedOffset = (short)(offset << (unsigned int)2);
        Cpu::programCounter = (unsigned int)(Cpu::programCounter + signedOffset) - 4;
        parseOpcode(read32(currentPC));
    }
}

void Cpu::addImmediateWord(unsigned int instruction) {
    auto items = parseData(instruction);
    auto rs = std::get<0>(items);
    auto rt = std::get<1>(items);
    auto immediate = std::get<2>(items);

    setRegister(rt, (unsigned int)(registers[rs] + (short)immediate));
}

void Cpu::setOnLessThanUnsigned(unsigned int instruction) {
    auto items = parseData(instruction);
    auto rs = std::get<0>(items);
    auto rt = std::get<1>(items);
    auto rd = std::get<2>(items) >> (unsigned int)11;

    if (registers[rs] < registers[rt]){
        setRegister(rd, (unsigned int)1);
    } else {
        setRegister(rd, (unsigned int)0);
    }
}

void Cpu::addUnsignedWord(unsigned int instruction) {
    auto items = parseData(instruction);
    auto rs = std::get<0>(items);
    auto rt = std::get<1>(items);
    auto rd = std::get<2>(items) >> (unsigned int)11;

    setRegister(rd, registers[rs] + registers[rt]);
    //add catch for overflow
}

void Cpu::storeHalfword(unsigned int instruction) {
    auto items = parseData(instruction);
    auto base = std::get<0>(items);
    auto rt = std::get<1>(items);
    auto offset = std::get<2>(items);

    if((coprocessors[0].registers[12] & (unsigned int)0x10000) != 0){
        //isolated cache
    } else {
        auto signedOffset = (short)offset;
        write16(registers[base] + (unsigned int)signedOffset, registers[rt] & (unsigned int)0x0000ffff);
    }
}

void Cpu::jumpAndLink(unsigned int instruction) {
    auto target = instruction & (unsigned int)0x3FFFFFF;

    Cpu::cycles++;
    auto currentPc = Cpu::programCounter;
    parseOpcode(read32(Cpu::programCounter));
    registers[31] = Cpu::programCounter;
    Cpu::programCounter -= 4;
    Cpu::programCounter = (Cpu::programCounter & 0xf0000000) | (target << (unsigned int)2);
}

void Cpu::andImmediate(unsigned int instruction) {
    auto items = parseData(instruction);
    auto rs = std::get<0>(items);
    auto rt = std::get<1>(items);
    auto immediate = std::get<2>(items);

    setRegister(rt, immediate & registers[rs]);
}

void Cpu::storeByte(unsigned int instruction) {
    auto items = parseData(instruction);
    auto base = std::get<0>(items);
    auto rt = std::get<1>(items);
    auto offset = std::get<2>(items);

    if ((coprocessors[0].registers[12] & (unsigned int)0x10000) != 0)
    {
        //isolated cache
    }
    else
    {
        auto signedOffset = (short)offset;
        write8(registers[base] + (unsigned int)signedOffset, registers[rt] & (unsigned int)0x000000ff);
    }
}

void Cpu::jumpRegister(unsigned int instruction) {
    auto items = parseData(instruction);
    auto rs = std::get<0>(items);

    Cpu::cycles++;
    parseOpcode(read32(Cpu::programCounter));
    Cpu::programCounter = registers[rs];
}

void Cpu::loadByte(unsigned int instruction) {
    auto items = parseData(instruction);
    auto base = std::get<0>(items);
    auto rt = std::get<1>(items);
    auto offset = std::get<2>(items);

    Cpu::cycles++;
    auto signedOffset = (short)offset;
    auto value = (int)((short)(read8(registers[base] + (unsigned int)signedOffset)));
    setRegister(rt, (unsigned int)value);

    parseOpcode(read32(Cpu::programCounter));
}

void Cpu::branchOnEqual(unsigned int instruction) {
    auto items = parseData(instruction);
    auto rs = std::get<0>(items);
    auto rt = std::get<1>(items);
    auto offset = std::get<2>(items);


    if (registers[rs] == registers[rt])
    {
        Cpu::cycles++;
        auto currentPc = Cpu::programCounter;
        auto signedOffset = (short)(offset << (unsigned int)2);
        Cpu::programCounter = (unsigned int)(Cpu::programCounter + signedOffset) - 4;
        parseOpcode(read32(currentPc));
    }
}

void Cpu::moveFromCoprocessor(coprocessor cop, unsigned int rt, unsigned int rd) {
    Cpu::cycles++;
    parseOpcode(read32(Cpu::programCounter));
    setRegister(rt, coprocessors[cop].registers[rd]);
}

void Cpu::_and(unsigned int instruction) {
    auto items = parseData(instruction);
    auto rs = std::get<0>(items);
    auto rt = std::get<1>(items);
    auto rd = std::get<2>(items) >> (unsigned int)11;

    setRegister(rd, registers[rs] & registers[rt]);
}

void Cpu::add(unsigned int instruction) {
    auto items = parseData(instruction);
    auto rs = std::get<0>(items);
    auto rt = std::get<1>(items);
    auto rd = std::get<2>(items) >> (unsigned int)11;

    setRegister(rd, registers[rs] + registers[rt]);
}

void Cpu::branchOnGreaterThanZero(unsigned int instruction) {
    auto items = parseData(instruction);
    auto rs = std::get<0>(items);
    auto offset = std::get<2>(items);

    if ((int)registers[rs] > (int)0)
    {
        cycles++;
        auto currentPc = programCounter;
        auto signedOffset = (short)(offset << (unsigned int)2);
        programCounter = (unsigned int)(programCounter + signedOffset) - 4;
        parseOpcode(read32(currentPc));
    }
}

void Cpu::branchOnLessThanOrEqualToZero(unsigned int instruction) {
    auto items = parseData(instruction);
    auto rs = std::get<0>(items);
    auto offset = std::get<2>(items);

    if ((int)registers[rs] <= (int)0)
    {
        cycles++;
        auto currentPc = programCounter;
        auto signedOffset = (short)(offset << (unsigned int)2);
        programCounter = (unsigned int)(programCounter + signedOffset) - 4;
        parseOpcode(read32(currentPc));
    }
}

void Cpu::loadByteUnsigned(unsigned int instruction) {
    auto items = parseData(instruction);
    auto base = std::get<0>(items);
    auto rt = std::get<1>(items);
    auto offset = std::get<2>(items);

    cycles++;
    auto signedOffset = (short)offset;
    auto value = (unsigned short)(read8(registers[base] + (unsigned int)signedOffset));
    setRegister(rt, (unsigned int)value);

    parseOpcode(read32(programCounter));
}

void Cpu::jumpAndLinkRegister(unsigned int instruction) {
    auto items = parseData(instruction);
    auto rs = std::get<0>(items);
    auto rd = std::get<2>(items) >> (unsigned int)11;

    cycles++;
    registers[rd] = programCounter + 4;
    registers[0] = 0;
    parseOpcode(read32(programCounter));
    programCounter = registers[rs];
}

void Cpu::branchInstruction(unsigned int instruction) {
    auto items = parseData(instruction);
    auto op = std::get<1>(items);

    if (op == 0b00001) {
        branchOnGreaterThanOrEqualToZero(items);
    } else if (op == 0b00000){
        branchOnLessThanZero(items);
    } else {
        throw std::exception();
    }
}

void Cpu::branchOnGreaterThanOrEqualToZero(std::tuple<Register, Register, unsigned int> items) {
    auto rs = std::get<0>(items);
    auto offset = std::get<2>(items);

    if ((int)registers[rs] >= (int)0)
    {
        cycles++;
        auto currentPc = programCounter;
        auto signedOffset = (short)(offset << (unsigned int)2);
        programCounter = (unsigned int)(programCounter + signedOffset) - 4;
        parseOpcode(read32(currentPc));
    }
}

void Cpu::branchOnLessThanZero(std::tuple<Register, Register, unsigned int> items) {
    auto rs = std::get<0>(items);
    auto offset = std::get<2>(items);

    if ((int)registers[rs] < (int)0)
    {
        cycles++;
        auto currentPc = programCounter;
        auto signedOffset = (short)(offset << (unsigned int)2);
        programCounter = (unsigned int)(programCounter + signedOffset) - 4;
        parseOpcode(read32(currentPc));
    }
}

void Cpu::setOnLessThanImmediate(unsigned int instruction) {
    auto items = parseData(instruction);
    auto rs = std::get<0>(items);
    auto rt = std::get<1>(items);
    auto immediate = std::get<2>(items);

    auto signedImm = (short)immediate;

    /*if(programCounter == 2147735308){
        registers[15] = 1;
    }*/

    if ((int)registers[rs] < signedImm) {
        setRegister(rt, (unsigned int)1);
    } else {
        setRegister(rt, (unsigned int)0);
    }
}

void Cpu::subtractUnsignedWord(unsigned int instruction) {
    auto items = parseData(instruction);
    auto rs = std::get<0>(items);
    auto rt = std::get<1>(items);
    auto rd = std::get<2>(items) >> (unsigned int)11;

    setRegister(rd, registers[rs] - registers[rt]);
}

void Cpu::shiftWordRightArithmetic(unsigned int instruction) {
    auto items = parseData(instruction);
    auto rt = std::get<1>(items);
    auto rd = std::get<2>(items) >> (unsigned int)11;
    auto sa = (std::get<2>(items) & (unsigned int)0x7ff) >> (unsigned int)6;

    int shiftedValue = (int)registers[rt] >> (int)sa;
    setRegister(rd, (unsigned int)shiftedValue);
}

void Cpu::divideWord(unsigned int instruction) {
    auto items = parseData(instruction);
    auto rs = std::get<0>(items);
    auto rt = std::get<1>(items);

    auto denominator = (int)registers[rt];
    auto numerator = (int)registers[rs];

    if(denominator == 0){
        HI = (unsigned int)numerator;

        if(numerator >= 0){
            LO = 0xffffffff;
        }else{
            LO = 1;
        }
    }
    else if ((unsigned int)numerator == 0x80000000 && (unsigned int)denominator == 0xffffffff){
        HI = 0;
        LO = 0x80000000;
    }else{
        LO = (unsigned int)(numerator / denominator);
        HI = (unsigned int)(numerator % denominator);
    }
}

void Cpu::moveFromLo(unsigned int instruction) {
    auto items = parseData(instruction);
    auto rd = std::get<2>(items) >> (unsigned int)11;

    setRegister(rd, LO);
}

void Cpu::shiftWordRightLogical(unsigned int instruction) {
    auto items = parseData(instruction);
    auto rt = std::get<1>(items);
    auto rd = std::get<2>(items) >> (unsigned int)11;
    auto sa = (std::get<2>(items) & (unsigned int)0x7ff) >> (unsigned int)6;

    setRegister(rd,registers[rt] >> (int)sa);
}

void Cpu::setOnLessThanImmediateUnsigned(unsigned int instruction) {
    auto items = parseData(instruction);
    auto rs = std::get<0>(items);
    auto rt = std::get<1>(items);
    auto immediate = std::get<2>(items);

    auto signedImm = (short)immediate;

    if(registers[rs] < (unsigned int)signedImm){
        setRegister(rt, 1);
    }else{
        setRegister(rt, 0);
    }
}

void Cpu::divideWordUnsigned(unsigned int instruction) {
    auto items = parseData(instruction);
    auto rs = std::get<0>(items);
    auto rt = std::get<1>(items);

    auto denominator = registers[rt];
    auto numerator = registers[rs];

    if (denominator == 0){
        HI = numerator;
        LO = 0xffffffff;
    }else{
        HI = numerator % denominator;
        LO = numerator / denominator;
    }
}

void Cpu::moveFromHi(unsigned int instruction) {
    auto items = parseData(instruction);
    auto rd = std::get<2>(items) >> (unsigned int)11;

    setRegister(rd, HI);
}

void Cpu::setOnLessThan(unsigned int instruction) {
    auto items = parseData(instruction);
    auto rs = std::get<0>(items);
    auto rt = std::get<1>(items);
    auto rd = std::get<2>(items) >> (unsigned int)11;

    if ((int)registers[rs] < (int)registers[rt]){
        setRegister(rd, 1);
    }else{
        setRegister(rd, 0);
    }
}

void Cpu::syscall(unsigned int instruction) {
    auto code = (instruction & 0x3ffffff) >> 6;

    coprocessors[0].registers[14] = programCounter - 4;

    unsigned int handlerAddress = 0;
    if ((coprocessors[0].registers[12] & (1 << 22)) != 0)
    {
        handlerAddress = 0xbfc00180;
    }
    else
    {
        handlerAddress = 0x80000080;
    }

    unsigned int mode = coprocessors[0].registers[12] & 0x3f;
    coprocessors[0].registers[12] &= ~(unsigned int)0x3f;
    coprocessors[0].registers[12] |= (mode << 2) & 0x3f;

    coprocessors[0].registers[13] = (unsigned int)(0x8 << 2); // exception cause is the exception instruction opcode

    programCounter = handlerAddress;
}

void Cpu::moveToLo(unsigned int instruction) {
    auto items = parseData(instruction);
    auto rs = std::get<0>(items);

    LO = registers[rs];
}

void Cpu::moveToHi(unsigned int instruction) {
    auto items = parseData(instruction);
    auto rs = std::get<0>(items);

    HI = registers[rs];
}

void Cpu::returnFromException() {
    unsigned int mode = coprocessors[0].registers[12] & 0x3f;
    coprocessors[0].registers[12] &= ~(unsigned int)0x3f;
    coprocessors[0].registers[12] |= mode >> 2;
}

void Cpu::storeWordLeft(unsigned int instruction) {
    auto items = parseData(instruction);
    auto base = std::get<0>(items);
    auto rt = std::get<1>(items);
    auto offset = std::get<2>(items);

    auto signedOffset = (short)offset;
    auto addr = registers[base] + (unsigned int)signedOffset;
    auto byteOffset = addr % 4;
    auto alignedAddr = addr - byteOffset;

    auto value = read32(alignedAddr);
    auto registerValue = registers[rt];

    switch (byteOffset)
    {
        case 0:
            write32(alignedAddr, (value & 0xffffff00) | (registerValue >> 24));
            break;
        case 1:
            write32(alignedAddr, (value & 0xffff0000) | (registerValue >> 16));
            break;
        case 2:
            write32(alignedAddr, (value & 0xff000000) | (registerValue >> 8));
            break;
        case 3:
            write32(alignedAddr, (value & 0x00000000) | registerValue);
            break;
        default:
            throw std::exception();
    }
}

void Cpu::shiftWordLeftLogicalVariable(unsigned int instruction) {
    auto items = parseData(instruction);
    auto rs = std::get<0>(items);
    auto rt = std::get<1>(items);
    auto rd = std::get<2>(items) >> (unsigned int)11;

    setRegister(rd, registers[rt] << (int)(registers[rs] & 0x1f));
}

void Cpu::loadHalfword(unsigned int instruction) {
    auto items = parseData(instruction);
    auto base = std::get<0>(items);
    auto rt = std::get<1>(items);
    auto offset = std::get<2>(items);

    cycles++;
    auto signedOffset = (short)offset;
    auto value = (short)(read16(registers[base] + (unsigned int)signedOffset));
    setRegister(rt, (unsigned int)value);

    parseOpcode(read32(programCounter));
}

void Cpu::nor(unsigned int instruction) {
    auto items = parseData(instruction);
    auto rs = std::get<0>(items);
    auto rt = std::get<1>(items);
    auto rd = std::get<2>(items) >> (unsigned int)11;

    setRegister(rd, ~(registers[rs] | registers[rt]));
}

void Cpu::_xor(unsigned int instruction) {
    auto items = parseData(instruction);
    auto rs = std::get<0>(items);
    auto rt = std::get<1>(items);
    auto rd = std::get<2>(items) >> (unsigned int)11;

    setRegister(rd, registers[rs] ^ registers[rt]);
}

void Cpu::multiplyUnsignedWord(unsigned int instruction) {
    auto items = parseData(instruction);
    auto rs = std::get<0>(items);
    auto rt = std::get<1>(items);

    auto product = (unsigned long)registers[rs] * (unsigned long)registers[rt];
    LO = (unsigned int)(product & 0xffffffff);
    HI = (unsigned int)((product & 0xffffffff00000000) >> 32);
}

void Cpu::loadWordLeft(unsigned int instruction) {
    auto items = parseData(instruction);
    auto base = std::get<0>(items);
    auto rt = std::get<1>(items);
    auto offset = std::get<2>(items);

    cycles++;
    auto signedOffset = (short)offset;
    auto addr = registers[base] + (unsigned int)signedOffset;
    auto byteOffset = addr % 4;
    auto alignedAddr = addr - byteOffset;

    auto value = read32(alignedAddr);
    auto registerValue = registers[rt];

    switch (byteOffset)
    {
        case 0:
            setRegister(rt, (registerValue & 0x00ffffff) | (value << 24));
            break;
        case 1:
            setRegister(rt, (registerValue & 0x0000ffff) | (value << 16));
            break;
        case 2:
            setRegister(rt, (registerValue & 0x000000ff) | (value << 8));
            break;
        case 3:
            setRegister(rt, (registerValue & 0x00000000) | value);
            break;
    }
}

void Cpu::shiftWordRightArithmeticVariable(unsigned int instruction) {
    auto items = parseData(instruction);
    auto rs = std::get<0>(items);
    auto rt = std::get<1>(items);
    auto rd = std::get<2>(items) >> 11;

    int shiftedValue = (int)registers[rt] >> (int)(registers[rs] & 0x1f);
    setRegister(rd, (unsigned int)shiftedValue);
}

void Cpu::shiftWordRightLogicalVariable(unsigned int instruction) {
    auto items = parseData(instruction);
    auto rs = std::get<0>(items);
    auto rt = std::get<1>(items);
    auto rd = std::get<2>(items) >> (unsigned int)11;

    setRegister(rd, registers[rt] >> (int)(registers[rs] & 0x1f));
}

void Cpu::loadWordRight(unsigned int instruction) {
    auto items = parseData(instruction);
    auto base = std::get<0>(items);
    auto rt = std::get<1>(items);
    auto offset = std::get<2>(items);

    cycles++;
    auto signedOffset = (short)offset;
    auto addr = registers[base] + (unsigned int)signedOffset;
    auto byteOffset = addr % 4;
    auto alignedAddr = addr - byteOffset;

    auto value = read32(alignedAddr);
    auto registerValue = registers[rt];

    switch (byteOffset)
    {
        case 0:
            setRegister(rt, (registerValue & 0x00000000) | value);
            break;
        case 1:
            setRegister(rt, (registerValue & 0xff000000) | (value >> 8));
            break;
        case 2:
            setRegister(rt, (registerValue & 0xffff0000) | (value >> 16));
            break;
        case 3:
            setRegister(rt, (registerValue & 0xffffff00) | (value >> 24));
            break;
    }
}

void Cpu::storeWordRight(unsigned int instruction) {
    auto items = parseData(instruction);
    auto base = std::get<0>(items);
    auto rt = std::get<1>(items);
    auto offset = std::get<2>(items);

    auto signedOffset = (short)offset;
    auto addr = registers[base] + (unsigned int)signedOffset;
    auto byteOffset = addr % 4;
    auto alignedAddr = addr - byteOffset;

    auto value = read32(alignedAddr);
    auto registerValue = registers[rt];

    switch (byteOffset)
    {
        case 3:
            write32(alignedAddr, (value & 0x00ffffff) | (registerValue << 24));
            break;
        case 2:
            write32(alignedAddr, (value & 0x0000ffff) | (registerValue << 16));
            break;
        case 1:
            write32(alignedAddr, (value & 0x000000ff) | (registerValue << 8));
            break;
        case 0:
            write32(alignedAddr, (value & 0x00000000) | registerValue);
            break;
    }
}

void Cpu::moveControlToCoprocessor(coprocessor cop, unsigned int rt, unsigned int rd) {
    if(cop == 2){
        coprocessors[cop].registers[32+rd] = registers[rt];
    }else{
        throw std::exception();
    }
}

void Cpu::xorImmediate(unsigned int instruction) {
    auto items = parseData(instruction);
    auto rs = std::get<0>(items);
    auto rt = std::get<1>(items);
    auto immediate = std::get<2>(items);

    setRegister(rt, registers[rs] ^ immediate);
}

void Cpu::setRegister(Register reg, unsigned int value) {
    if(reg == 4 && value == 26 && cycles >= 100000000){
        auto c = 0;
        //registers[reg] = c;
        registers[reg] = value;
        registers[0] = 0;
    }

    if(reg == 9 && value == 0x000a){
        auto e = 0;
        //registers[reg] = c;
        registers[reg] = value;
        registers[0] = 0;
    }

    if(reg == 3 && value == 26 && cycles >= 100000000){
        auto d = 0;
        //registers[reg] = c;
        registers[reg] = value;
        registers[0] = 0;

    }else{
        registers[reg] = value;
        registers[0] = 0;
    }
}
