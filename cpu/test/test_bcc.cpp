#include "catch2/catch_all.hpp"
#include <stdio.h>

#include "cpu.h"
#include "ram.h"

#define BCC_REL 0x90

TEST_CASE("BCC Instruction", "[BCC]") {

    CPU6502 cpu;
    RAM ram;
    cpu.connect(ram);

    SECTION("Branch Taken (Carry Clear) - Forward") {
        cpu.regs.PC = 0x1000;
        cpu.regs.flags.C = 0;

        ram.write(cpu.regs.PC, BCC_REL);
        ram.write(cpu.regs.PC+1, 0x05); // Branch forward by 5
        ram.write(cpu.regs.PC+2, 0x70); // does not matter
        cpu.execute();

        REQUIRE(cpu.regs.PC == 0x1008); // PC branched forward by 5 + 2
    }

    SECTION("Branch Taken (Carry Clear) - Backward") {
        cpu.regs.PC = 0x1005;
        cpu.regs.flags.C = 0;

        ram.write(cpu.regs.PC, BCC_REL);
        ram.write(cpu.regs.PC+1, 0xFB); // Branch backward by 5 (0xFB is -5 in two's complement)
        ram.write(cpu.regs.PC+2, 0x70); // does not matter 
        cpu.execute();

        REQUIRE(cpu.regs.PC == 0x1003); // PC branched backward by 5
    }

    SECTION("Branch Not Taken (Carry Set)") {
        cpu.regs.PC = 0x1000;
        cpu.regs.flags.C = 1;

        ram.write(cpu.regs.PC, BCC_REL);
        ram.write(cpu.regs.PC+1, 0x05); // does not matter
        ram.write(cpu.regs.PC+2, 0x70); // does not matter
        cpu.execute();

        REQUIRE(cpu.regs.PC == 0x1003); // PC incremented by 2 (no branch taken)
    }
}