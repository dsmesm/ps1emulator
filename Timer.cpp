#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"
//
// Created by Dima on 9/8/2020.
//

#include <exception>
#include "Timer.h"
#include "Interrupts.h"

unsigned int Timer::getMode() {
    unsigned int mode = 0;

    mode |= syncEnable;
    mode |= (syncMode << 1);
    mode |= (reset << 3);
    mode |= (targetReachedIRQ << 4);
    mode |= (maxReachedIRQ << 5);
    mode |= (repeatMode << 6);
    mode |= (pulseMode << 7);
    mode |= (clockSource << 8);
    mode |= (IRQ << 10);
    mode |= (targetReached << 11);
    mode |= (maxReached << 12);

    targetReached = 0;
    maxReached = 0;

    return mode;
}

void Timer::setMode(unsigned int value) {
    syncEnable = value & 0x1;
    syncMode = (value >> 1) & 0x3;
    reset = (value >> 3) & 0x1;
    targetReachedIRQ = (value >> 4) & 0x1;
    maxReachedIRQ = (value >> 5) & 0x1;
    repeatMode = (value >> 6) & 0x1;
    pulseMode = (value >> 7) & 0x1;
    clockSource = (value >> 8) & 0x3;
    //IRQ = (value >> 10) & 0x1;
    IRQ = 1;
    //targetReached = (value >> 11) & 0x1;
    //maxReached = (value >> 12) & 0x1;

    currentValue = 0;
}

void Timer::incrementTimer2() {
    /*if(IRQ == 0){
        if(pulseMode == 0){
            if(pulseIrqWindow == 5){
                IRQ = 1;
                pulseIrqWindow = 0;
            }
            pulseIrqWindow++;
        }else{

        }
    }*/

    switch (syncEnable) {
        case 0:

            switch (reset) {
                case 1:
                    if (currentValue == targetValue) {
                        if(targetReachedIRQ == 1){
                            IRQ = 0;
                            Interrupts::timer2 = 1;
                        }
                        targetReached = 1;
                        currentValue = 0;
                    } else {
                        currentValue++;
                    }
                    break;
                case 0:
                    if (currentValue == 0xffff) {
                        if(maxReachedIRQ == 1){
                            IRQ = 0;
                            Interrupts::timer2 = 1;
                        }
                        maxReached = 1;
                        currentValue = 0;
                    } else {
                        currentValue++;
                    }
                    break;
                default: throw std::exception();
            }
            break;

        case 1:

            switch (syncMode) {
                case 0:
                case 3:
                    //timer stopped, no increment
                    break;
                case 1:
                case 2:
                    switch (reset) {
                        case 1:
                            if (currentValue == targetValue) {
                                if(targetReachedIRQ == 1){
                                    IRQ = 0;
                                    Interrupts::timer2 = 1;
                                }
                                targetReached = 1;
                                currentValue = 0;
                            } else {
                                currentValue++;
                            }
                            break;
                        case 0:
                            if (currentValue == 0xffff) {
                                if(maxReachedIRQ == 1){
                                    IRQ = 0;
                                    Interrupts::timer2 = 1;
                                }
                                maxReached = 1;
                                currentValue = 0;
                            } else {
                                currentValue++;
                            }
                            break;
                        default: throw std::exception();
                    }
                    break;
            }
            break;
        default: throw std::exception();
    }
}

void Timer::incrementTimer0(bool hblank) {
    switch (syncEnable) {
        case 0:

            switch (reset) {
                case 1:
                    if (currentValue == targetValue) {
                        if(targetReachedIRQ == 1){
                            IRQ = 0;
                            Interrupts::timer0 = 1;
                        }
                        targetReached = 1;
                        currentValue = 0;
                    } else {
                        currentValue++;
                    }
                    break;
                case 0:
                    if (currentValue == 0xffff) {
                        if(maxReachedIRQ == 1){
                            IRQ = 0;
                            Interrupts::timer0 = 1;
                        }
                        maxReached = 1;
                        currentValue = 0;
                    } else {
                        currentValue++;
                    }
                    break;
                default: throw std::exception();
            }
            break;

        case 1:

            switch (syncMode) {
                case 0:
                    if (!hblank){

                        switch (reset) {
                            case 1:
                                if (currentValue == targetValue) {
                                    if(targetReachedIRQ == 1){
                                        IRQ = 0;
                                        Interrupts::timer0 = 1;
                                    }
                                    targetReached = 1;
                                    currentValue = 0;
                                } else {
                                    currentValue++;
                                }
                                break;
                            case 0:
                                if (currentValue == 0xffff) {
                                    if(maxReachedIRQ == 1){
                                        IRQ = 0;
                                        Interrupts::timer0 = 1;
                                    }
                                    maxReached = 1;
                                    currentValue = 0;
                                } else {
                                    currentValue++;
                                }
                                break;
                            default: throw std::exception();
                        }
                    }
                    break;
                case 1:
                    if (hblank && !enteringHblank){
                        enteringHblank = true;
                        currentValue = 0;
                    } else if (hblank && enteringHblank){
                        switch (reset) {
                            case 1:
                                if (currentValue == targetValue) {
                                    if(targetReachedIRQ == 1){
                                        IRQ = 0;
                                        Interrupts::timer0 = 1;
                                    }
                                    targetReached = 1;
                                    currentValue = 0;
                                } else {
                                    currentValue++;
                                }
                                break;
                            case 0:
                                if (currentValue == 0xffff) {
                                    if(maxReachedIRQ == 1){
                                        IRQ = 0;
                                        Interrupts::timer0 = 1;
                                    }
                                    maxReached = 1;
                                    currentValue = 0;
                                } else {
                                    currentValue++;
                                }
                                break;
                            default: throw std::exception();
                        }
                    } else {
                        enteringHblank = false;
                        switch (reset) {
                            case 1:
                                if (currentValue == targetValue) {
                                    if(targetReachedIRQ == 1){
                                        IRQ = 0;
                                        Interrupts::timer0 = 1;
                                    }
                                    targetReached = 1;
                                    currentValue = 0;
                                } else {
                                    currentValue++;
                                }
                                break;
                            case 0:
                                if (currentValue == 0xffff) {
                                    if(maxReachedIRQ == 1){
                                        IRQ = 0;
                                        Interrupts::timer0 = 1;
                                    }
                                    maxReached = 1;
                                    currentValue = 0;
                                } else {
                                    currentValue++;
                                }
                                break;
                            default: throw std::exception();
                        }
                    }

                    break;
                case 2:
                    if (hblank && !enteringHblank){
                        enteringHblank = true;
                        currentValue = 0;
                    } else if (hblank && enteringHblank){
                        switch (reset) {
                            case 1:
                                if (currentValue == targetValue) {
                                    if(targetReachedIRQ == 1){
                                        IRQ = 0;
                                        Interrupts::timer0 = 1;
                                    }
                                    targetReached = 1;
                                    currentValue = 0;
                                } else {
                                    currentValue++;
                                }
                                break;
                            case 0:
                                if (currentValue == 0xffff) {
                                    if(maxReachedIRQ == 1){
                                        IRQ = 0;
                                        Interrupts::timer0 = 1;
                                    }
                                    maxReached = 1;
                                    currentValue = 0;
                                } else {
                                    currentValue++;
                                }
                                break;
                            default: throw std::exception();
                        }
                    } else {
                        enteringHblank = false;
                    }

                    break;

                case 3:
                    if (hblank){
                        syncEnable = 1;
                    }

                    break;
            }
            break;
        default: throw std::exception();
    }
}

void Timer::incrementTimer1(bool vblank) {
    switch (syncEnable) {
        case 0:

            switch (reset) {
                case 1:
                    if (currentValue == targetValue) {
                        if(targetReachedIRQ == 1){
                            IRQ = 0;
                            Interrupts::timer1 = 1;
                        }
                        targetReached = 1;
                        currentValue = 0;
                    } else {
                        currentValue++;
                    }
                    break;
                case 0:
                    if (currentValue == 0xffff) {
                        if(maxReachedIRQ == 1){
                            IRQ = 0;
                            Interrupts::timer1 = 1;
                        }
                        maxReached = 1;
                        currentValue = 0;
                    } else {
                        currentValue++;
                    }
                    break;
                default: throw std::exception();
            }
            break;

        case 1:

            switch (syncMode) {
                case 0:
                    if (!vblank){

                        switch (reset) {
                            case 1:
                                if (currentValue == targetValue) {
                                    if(targetReachedIRQ == 1){
                                        IRQ = 0;
                                        Interrupts::timer1 = 1;
                                    }
                                    targetReached = 1;
                                    currentValue = 0;
                                } else {
                                    currentValue++;
                                }
                                break;
                            case 0:
                                if (currentValue == 0xffff) {
                                    if(maxReachedIRQ == 1){
                                        IRQ = 0;
                                        Interrupts::timer1 = 1;
                                    }
                                    maxReached = 1;
                                    currentValue = 0;
                                } else {
                                    currentValue++;
                                }
                                break;
                            default: throw std::exception();
                        }
                    }
                    break;
                case 1:
                    if (vblank && !enteringHblank){
                        enteringHblank = true;
                        currentValue = 0;
                    } else if (vblank && enteringHblank){
                        switch (reset) {
                            case 1:
                                if (currentValue == targetValue) {
                                    if(targetReachedIRQ == 1){
                                        IRQ = 0;
                                        Interrupts::timer1 = 1;
                                    }
                                    targetReached = 1;
                                    currentValue = 0;
                                } else {
                                    currentValue++;
                                }
                                break;
                            case 0:
                                if (currentValue == 0xffff) {
                                    if(maxReachedIRQ == 1){
                                        IRQ = 0;
                                        Interrupts::timer1 = 1;
                                    }
                                    maxReached = 1;
                                    currentValue = 0;
                                } else {
                                    currentValue++;
                                }
                                break;
                            default: throw std::exception();
                        }
                    } else {
                        enteringHblank = false;
                        switch (reset) {
                            case 1:
                                if (currentValue == targetValue) {
                                    if(targetReachedIRQ == 1){
                                        IRQ = 0;
                                        Interrupts::timer1 = 1;
                                    }
                                    targetReached = 1;
                                    currentValue = 0;
                                } else {
                                    currentValue++;
                                }
                                break;
                            case 0:
                                if (currentValue == 0xffff) {
                                    if(maxReachedIRQ == 1){
                                        IRQ = 0;
                                        Interrupts::timer1 = 1;
                                    }
                                    maxReached = 1;
                                    currentValue = 0;
                                } else {
                                    currentValue++;
                                }
                                break;
                            default: throw std::exception();
                        }
                    }

                    break;
                case 2:
                    if (vblank && !enteringHblank){
                        enteringHblank = true;
                        currentValue = 0;
                    } else if (vblank && enteringHblank){
                        switch (reset) {
                            case 1:
                                if (currentValue == targetValue) {
                                    if(targetReachedIRQ == 1){
                                        IRQ = 0;
                                        Interrupts::timer1 = 1;
                                    }
                                    targetReached = 1;
                                    currentValue = 0;
                                } else {
                                    currentValue++;
                                }
                                break;
                            case 0:
                                if (currentValue == 0xffff) {
                                    if(maxReachedIRQ == 1){
                                        IRQ = 0;
                                        Interrupts::timer1 = 1;
                                    }
                                    maxReached = 1;
                                    currentValue = 0;
                                } else {
                                    currentValue++;
                                }
                                break;
                            default: throw std::exception();
                        }
                    } else {
                        enteringHblank = false;
                    }

                    break;

                case 3:
                    if (vblank){
                        syncEnable = 1;
                    }

                    break;
            }
            break;
        default: throw std::exception();
    }
}

#pragma clang diagnostic pop