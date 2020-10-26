//
// Created by Dima on 10/16/2020.
//

#ifndef PS1EMULATOR_CONTROLLER_H
#define PS1EMULATOR_CONTROLLER_H


struct Controller {
    unsigned int txEnable = 0;
    unsigned int output = 0;
    unsigned int rxEnable = 0;
    unsigned int acknowledge = 0;
    unsigned int reset = 0;
    unsigned int rxInterruptMode = 0;
    unsigned int txInterruptEnable = 0;
    unsigned int rxInterruptEnable = 0;
    unsigned int acknowledgeInterruptEnable = 0;
    unsigned int desiredSlot = 0;

    unsigned int baudRateReload = 0;

    unsigned int baudRateReloadFactor = 0;
    unsigned int characterLength = 0;
    unsigned int parityEnable = 0;
    unsigned int parityType = 0;
    unsigned int clockPolarity = 0;

    void setControl(unsigned int value);
    void setMode(unsigned int value);
};


#endif //PS1EMULATOR_CONTROLLER_H
