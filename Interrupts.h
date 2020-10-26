//
// Created by Dima on 9/7/2020.
//

#ifndef PS1EMULATOR_INTERRUPTS_H
#define PS1EMULATOR_INTERRUPTS_H


struct Interrupts {
    static unsigned int vblank;
    static unsigned int gpu;
    static unsigned int cdrom;
    static unsigned int dma;
    static unsigned int timer0;
    static unsigned int timer1;
    static unsigned int timer2;
    static unsigned int memcard; // also controller
    static unsigned int sio;
    static unsigned int spu;
    static unsigned int controller;

    static unsigned int vblankMask;
    static unsigned int gpuMask;
    static unsigned int cdromMask;
    static unsigned int dmaMask;
    static unsigned int timer0Mask;
    static unsigned int timer1Mask;
    static unsigned int timer2Mask;
    static unsigned int memcardMask; // also controller
    static unsigned int sioMask;
    static unsigned int spuMask;
    static unsigned int controllerMask;

    static unsigned int getInterruptStatus();
    static void setInterruptStatus(unsigned int value);

    static unsigned int getInterruptMask();
    static void setInterruptMask(unsigned int value);
};


#endif //PS1EMULATOR_INTERRUPTS_H
