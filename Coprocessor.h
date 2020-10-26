//
// Created by Dima on 9/2/2020.
//

#ifndef PS1EMULATOR_COPROCESSOR_H
#define PS1EMULATOR_COPROCESSOR_H


#include <vector>

typedef unsigned int Register;

struct Coprocessor {
    std::vector<Register> registers;
    Coprocessor(int registerCount);
    Coprocessor();
};


#endif //PS1EMULATOR_COPROCESSOR_H
