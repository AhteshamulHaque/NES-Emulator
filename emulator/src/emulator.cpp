#include <iostream>
#include "cpu.h"
#include "ram.h"
#include "emulator.h"

int main() {
    CPU6502 cpu; 
    RAM ram;
    
    cpu.connect(ram);

    cpu.execute();
    std::cout << "A = 0x" << std::hex << cpu.regs.PC << std::endl;
    return 0;
}

