#include <iostream>
#include <chrono>
#include "Bios.h"
#include "Cpu.h"
#include "Renderer.h"


static Cpu cpu;

void init(){
    cpu = Cpu();

    Renderer::init();
}


int main() {
    init();

    auto start = std::chrono::high_resolution_clock::now();

    try {

        for (;;) {
            unsigned int data = cpu.read32(Cpu::programCounter);
            cpu.parseOpcode(data);
            Cpu::cycles++;
        }
    } catch (std::exception &e) {
        auto finish = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = finish - start;
        std::cout << "Elapsed time: " << elapsed.count() << " s\n";
        std::cout << "Cycle count: " << Cpu::cycles << " cycles\n";
    }
}

