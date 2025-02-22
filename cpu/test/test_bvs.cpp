#include "catch2/catch_all.hpp"

#include "cpu.h"
#include "ram.h"

#define BVS_REL 0x70

TEST_CASE("BVS Instruction", "[BVS]") {

    CPU6502 cpu;
    RAM ram;
    cpu.connect(ram);

    SECTION("Branch Taken (Carry Set) - Forward") {
        cpu.regs.PC = 0x1000;
        cpu.regs.flags.V = 1;

        ram.write(cpu.regs.PC, BVS_REL);
        ram.write(cpu.regs.PC+1, 0x05); // Branch forward by 5
        ram.write(cpu.regs.PC+2, 0x70); // does not matter
        cpu.execute();

        REQUIRE(cpu.regs.PC == 0x1008); // PC branched forward by 5 + 2
    }

    SECTION("Branch Taken (Carry Set) - Backward") {
        cpu.regs.PC = 0x1005;
        cpu.regs.flags.V = 1;

        ram.write(cpu.regs.PC, BVS_REL);
        ram.write(cpu.regs.PC+1, 0xFB); // Branch backward by 5 (0xFB is -5 in two's complement)
        ram.write(cpu.regs.PC+2, 0x70); // does not matter 
        cpu.execute();

        REQUIRE(cpu.regs.PC == 0x1003); // PC branched backward by 5
    }

    SECTION("Branch Not Taken (Carry Clear)") {
        cpu.regs.PC = 0x1000;
        cpu.regs.flags.V = 0;

        ram.write(cpu.regs.PC, BVS_REL);
        ram.write(cpu.regs.PC+1, 0x05); // does not matter
        ram.write(cpu.regs.PC+2, 0x70); // does not matter
        cpu.execute();

        REQUIRE(cpu.regs.PC == 0x1003); // PC incremented by 2 (no branch taken)
    }
}