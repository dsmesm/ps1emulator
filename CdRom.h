//
// Created by Dima on 10/10/2020.
//

#ifndef PS1EMULATOR_CDROM_H
#define PS1EMULATOR_CDROM_H


#include <queue>
#include <unordered_map>

struct CdRom {
    CdRom();

    std::unordered_map<unsigned int, void(CdRom::*)()> cdromCommands;

    unsigned int portIndex = 0;
    unsigned int xaAdpcmFifoEmpty = 0;
    unsigned int parameterFifoEmpty = 1; // bound to timers
    unsigned int parameterFifoFull = 1; //
    unsigned int responseFifoEmpty = 0; //
    unsigned int dataFifoEmpty = 0;
    unsigned int busy = 0;

    unsigned int commandStart = 0;

    unsigned int interruptEnable = 0;

    unsigned int responsesLeft = 0;
    bool firstResponseReceived = false;

    unsigned int playing = 0;
    unsigned int seeking = 0;
    unsigned int reading = 0;
    unsigned int shellOpen = 0;
    unsigned int idError = 0;
    unsigned int seekError = 0;
    unsigned int motor = 1; // leave motor on by default
    unsigned int error = 0;

    std::queue<unsigned int> responseQueue;
    std::queue<unsigned int> parameterQueue;
    std::queue<unsigned int> commandQueue;

    unsigned int getStatusRegister();
    void setStatusRegister(unsigned int value);

    void setInterruptFlag(unsigned int value);
    unsigned int getInterruptFlag();

    unsigned int getStatusCode();

    void executeCommand();

    void test();
    void getStat();
    void getId();
};


#endif //PS1EMULATOR_CDROM_H
