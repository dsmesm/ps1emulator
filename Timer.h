//
// Created by Dima on 9/8/2020.
//

#ifndef PS1EMULATOR_TIMER_H
#define PS1EMULATOR_TIMER_H


struct Timer {
    unsigned int pulseIrqWindow = 0;
    bool enteringHblank = false;
    bool enteringVblank = false;
    unsigned int currentValue = 0;

    //unsigned int mode = 0;

    void setMode(unsigned int value);
    unsigned int getMode();

    //counter mode register
    unsigned int syncEnable = 0;
    unsigned int syncMode = 0;
    unsigned int reset = 0;
    unsigned int targetReachedIRQ = 0;
    unsigned int maxReachedIRQ = 0;
    unsigned int repeatMode = 0;
    unsigned int pulseMode = 0;
    unsigned int clockSource = 0;
    unsigned int IRQ = 0;
    unsigned int targetReached = 0;
    unsigned int maxReached = 0;

    unsigned int targetValue = 0;

    void incrementTimer0(bool hblank);
    void incrementTimer1(bool vblank);
    void incrementTimer2();
};


#endif //PS1EMULATOR_TIMER_H
