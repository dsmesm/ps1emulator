#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"
//
// Created by Dima on 10/16/2020.
//

#include "Controller.h"

void Controller::setControl(unsigned int value) {
    txEnable = value & 0x1;
    output = (value >> 1) & 0x1;
    rxEnable = (value >> 2) & 0x1;
    acknowledge = (value >> 4) & 0x1;
    reset = (value >> 6) & 0x1;
    rxInterruptMode = (value >> 8) & 0x3;
    txInterruptEnable = (value >> 10) & 0x1;
    rxInterruptEnable = (value >> 11) & 0x1;
    acknowledgeInterruptEnable = (value >> 12) & 0x1;
    desiredSlot = (value >> 13) & 0x1;
}

void Controller::setMode(unsigned int value) {
    baudRateReloadFactor = value & 0x3;
    characterLength = (value >> 2) & 0x3;
    parityEnable = (value >> 4) & 0x1;
    parityType = (value >> 5) & 0x1;
    clockPolarity = (value >> 8) & 0x1;
}

#pragma clang diagnostic pop