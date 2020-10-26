//
// Created by Dima on 9/2/2020.
//

#include "Coprocessor.h"

Coprocessor::Coprocessor(int registerCount) {
    registers = std::vector<Register>(registerCount);
}

Coprocessor::Coprocessor() {

}
