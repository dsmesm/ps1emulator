//
// Created by Dima on 9/7/2020.
//

#include "Interrupts.h"
#include "HardwareRegisters.h"

unsigned int Interrupts::vblank = 0;
unsigned int Interrupts::gpu = 0;
unsigned int Interrupts::cdrom = 0;
unsigned int Interrupts::dma = 0;
unsigned int Interrupts::timer0 = 0;
unsigned int Interrupts::timer1 = 0;
unsigned int Interrupts::timer2 = 0;
unsigned int Interrupts::memcard = 0;
unsigned int Interrupts::sio = 0;
unsigned int Interrupts::spu = 0;
unsigned int Interrupts::controller = 0;

unsigned int Interrupts::vblankMask = 0;
unsigned int Interrupts::gpuMask = 0;
unsigned int Interrupts::cdromMask = 0;
unsigned int Interrupts::dmaMask = 0;
unsigned int Interrupts::timer0Mask = 0;
unsigned int Interrupts::timer1Mask = 0;
unsigned int Interrupts::timer2Mask = 0;
unsigned int Interrupts::memcardMask = 0;
unsigned int Interrupts::sioMask = 0;
unsigned int Interrupts::spuMask = 0;
unsigned int Interrupts::controllerMask = 0;

unsigned int Interrupts::getInterruptStatus() {
    unsigned int interruptStatus = 0;

    interruptStatus |= vblank;
    interruptStatus |= (gpu << (unsigned int)1);
    interruptStatus |= (cdrom << (unsigned int)2);
    interruptStatus |= (dma << (unsigned int)3);
    interruptStatus |= (timer0 << (unsigned int)4);
    interruptStatus |= (timer1 << (unsigned int)5);
    interruptStatus |= (timer2 << (unsigned int)6);
    interruptStatus |= (memcard << (unsigned int)7);
    interruptStatus |= (sio << (unsigned int)8);
    interruptStatus |= (spu << (unsigned int)9);
    interruptStatus |= (controller << (unsigned int)10);

    return interruptStatus;
}

void Interrupts::setInterruptStatus(unsigned int value) {
    vblank &= value & (unsigned int)0x1;
    gpu &= (value >> (unsigned int)1) & (unsigned int)0x1;
    cdrom &= (value >> (unsigned int)2) & (unsigned int)0x1;
    dma &= (value >> (unsigned int)3) & (unsigned int)0x1;
    timer0 &= (value >> (unsigned int)4) & (unsigned int)0x1;
    timer1 &= (value >> (unsigned int)5) & (unsigned int)0x1;
    timer2 &= (value >> (unsigned int)6) & (unsigned int)0x1;
    memcard &= (value >> (unsigned int)7) & (unsigned int)0x1;
    sio &= (value >> (unsigned int)8) & (unsigned int)0x1;
    spu &= (value >> (unsigned int)9) & (unsigned int)0x1;
    controller &= (value >> (unsigned int)10) & (unsigned int)0x1;

    if(HardwareRegisters::cdrom.responsesLeft != 0){
        cdrom = 1;
    }

    //add cdrom multi-response support
}

unsigned int Interrupts::getInterruptMask() {
    unsigned int interruptMask = 0;

    interruptMask |= vblankMask;
    interruptMask |= (gpuMask << (unsigned int)1);
    interruptMask |= (cdromMask << (unsigned int)2);
    interruptMask |= (dmaMask << (unsigned int)3);
    interruptMask |= (timer0Mask << (unsigned int)4);
    interruptMask |= (timer1Mask << (unsigned int)5);
    interruptMask |= (timer2Mask << (unsigned int)6);
    interruptMask |= (memcardMask << (unsigned int)7);
    interruptMask |= (sioMask << (unsigned int)8);
    interruptMask |= (spuMask << (unsigned int)9);
    interruptMask |= (controllerMask << (unsigned int)10);

    return interruptMask;
}

void Interrupts::setInterruptMask(unsigned int value) {
    vblankMask = value & (unsigned int)0x1;
    gpuMask = (value >> (unsigned int)1) & (unsigned int)0x1;
    cdromMask = (value >> (unsigned int)2) & (unsigned int)0x1;
    dmaMask = (value >> (unsigned int)3) & (unsigned int)0x1;
    timer0Mask = (value >> (unsigned int)4) & (unsigned int)0x1;
    timer1Mask = (value >> (unsigned int)5) & (unsigned int)0x1;
    timer2Mask = (value >> (unsigned int)6) & (unsigned int)0x1;
    memcardMask = (value >> (unsigned int)7) & (unsigned int)0x1;
    sioMask = (value >> (unsigned int)8) & (unsigned int)0x1;
    spuMask = (value >> (unsigned int)9) & (unsigned int)0x1;
    controllerMask = (value >> (unsigned int)10) & (unsigned int)0x1;
}
