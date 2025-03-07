#include <iostream>
#include "cpu.h"
#include "ppu.h"
#include "ram.h"
#include "emulator.h"

int main() {
    CPU6502 cpu; 
    PPU ppu;
    RAM ram;
    
    cpu.connect(ram);
    cpu.execute();
    ppu.run();

    return 0;
}

