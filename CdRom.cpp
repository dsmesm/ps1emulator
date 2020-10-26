#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"
//
// Created by Dima on 10/10/2020.
//

#include <exception>
#include "CdRom.h"
#include "Interrupts.h"

void CdRom::setStatusRegister(unsigned int value) {
    portIndex = value & 0x3;
    xaAdpcmFifoEmpty = (value >> 2) & 0x1;
    //parameterFifoEmpty = (value >> 3) & 0x1;
    //parameterFifoFull = (value >> 4) & 0x1;
    //responseFifoEmpty = (value >> 5) & 0x1;
    dataFifoEmpty = (value >> 6) & 0x1;
    busy = (value >> 7) & 0x1;
}

void CdRom::setInterruptFlag(unsigned int value) {
    if (value == 0x1f)
    {
        parameterFifoEmpty = 1;// empty
        parameterFifoFull = 1; // empty
        responseFifoEmpty = 0;

        std::queue<unsigned int> empty;
        std::swap(responseQueue, empty);

        //interruptEnable = 0x0;
    }
    else if(value == 0x07)
    {
        //interruptEnable = 0x0;
    }
    else if (value == 0x40) // 64 dec
    {
        parameterFifoEmpty = 1;// empty
        parameterFifoFull = 1; // empty
        responseFifoEmpty = 0;

        std::queue<unsigned int> empty;
        std::swap(responseQueue, empty);
    }
    else
    {
        throw std::exception();
    }
}

unsigned int CdRom::getStatusRegister() {
    unsigned int statReg = 0;

    statReg |= portIndex;
    statReg |= xaAdpcmFifoEmpty << 2;
    statReg |= parameterFifoEmpty << 3;
    statReg |= parameterFifoFull << 4;
    statReg |= responseFifoEmpty << 5;
    statReg |= dataFifoEmpty << 6;
    statReg |= busy << 7;

    return statReg;
}

void CdRom::executeCommand() {
    auto command = commandQueue.front();
    commandQueue.pop();

    (this->*cdromCommands[command])();
}

CdRom::CdRom() {
    cdromCommands = {
            {0x19, &CdRom::test},
            {0x01, &CdRom::getStat},
            {0x1a, &CdRom::getId},
    };
}

void CdRom::test() {
    auto parameter = parameterQueue.front();
    parameterQueue.pop();

    if (parameter == 0x20){
        //94h,09h,19h,C0h
        responseQueue.push(0x94);
        responseQueue.push(0x09);
        responseQueue.push(0x19);
        responseQueue.push(0xC0);

        responseFifoEmpty = 1;
        parameterFifoEmpty = 1;

        //interruptEnable = 0x1f;
        Interrupts::cdrom = 1;
    }else{
        throw std::exception();
    }
}

unsigned int CdRom::getInterruptFlag() {
    if (!responseQueue.empty())
    {
        if (responsesLeft == 1)
        {
            if (firstResponseReceived)
            {
                //Interrupts::cdrom = 1;
                responsesLeft--;
                /*interruptEnable = 5;
                error = 1;
                return (unsigned int)((5 | (commandStart << 4)) | (0b111 << 5)); // no disk*/
                //interruptEnable = 2;
                error = 0;
                return (unsigned int)((2 | (commandStart << 4)) | (0b111 << 5)); // ntsc disk
            }
            else
            {
                //Interrupts::cdrom = 1;
                firstResponseReceived = true;
                //interruptEnable = 3;
                return (unsigned int)((3 | (commandStart << 4)) | (0b111 << 5));
            }
        }
        else if (responsesLeft == 0)
        {
            //Interrupts::cdrom = 1;
            //interruptEnable = 3;
            return (unsigned int)((3 | (commandStart << 4)) | (0b111 << 5));
        }
        else
        {
            throw std::exception();
        }

    }
    else if (responseQueue.empty())
    {
        //interruptEnable = 0;
        return (unsigned int)((0 | (commandStart << 4)) | (0b111 << 5));
    }
    else
    {
        throw std::exception();
    }
}

void CdRom::getStat() {
    responseQueue.push(getStatusCode());

    responseFifoEmpty = 1;
    parameterFifoEmpty = 1;

    //interruptEnable = 0x1f;
    Interrupts::cdrom = 1;
}

void CdRom::getId() {
    firstResponseReceived = false;

    /*responseQueue.push(getStatusCode()); // no disk
    responseQueue.push(0x08);
    responseQueue.push(0x40);

    responseQueue.push(0x00);
    responseQueue.push(0x00);

    responseQueue.push(0x00);
    responseQueue.push(0x00);

    responseQueue.push(0x00);
    responseQueue.push(0x00);*/

    responseQueue.push(getStatusCode()); // ntsc disk
    responseQueue.push(getStatusCode());
    responseQueue.push(0x00);

    responseQueue.push(0x20);
    responseQueue.push(0x00);

    responseQueue.push(0x53);
    responseQueue.push(0x43);

    responseQueue.push(0x45);
    responseQueue.push(0x41);

    responsesLeft = 1;

    responseFifoEmpty = 1;
    parameterFifoEmpty = 1;

    //interruptEnable = 0x1f;
    Interrupts::cdrom = 1;
}

unsigned int CdRom::getStatusCode() {
    unsigned int statusCode = 0;

    statusCode |= playing << 7;
    statusCode |= seeking << 6;
    statusCode |= reading << 5;
    statusCode |= shellOpen << 4;
    statusCode |= idError << 3;
    statusCode |= seekError << 2;
    statusCode |= motor << 1;
    statusCode |= error;

    return statusCode;
}

#pragma clang diagnostic pop