#include "catch2/catch_all.hpp"

#include "cpu.h"
#include "ram.h"

#define JMP_ABS 0x4C
#define JMP_IND 0x6C

TEST_CASE("JMP Instruction", "[JMP]") {

    CPU6502 cpu;
    RAM ram;
    cpu.connect(ram);

    SECTION("Normal Absolute Jump") {
        cpu.regs.PC = 0x1000;
        uint16_t target_addr = 0x2050;

        ram.write(cpu.regs.PC, JMP_ABS);
        ram.write(cpu.regs.PC+1, 0x50);   // Low byte of target address
        ram.write(cpu.regs.PC+2,0x20); // High byte of target address
        cpu.execute();

        REQUIRE(cpu.regs.PC == target_addr);
    }

    SECTION("Normal Indirect Jump") {
        cpu.regs.PC = 0x1000;
        uint16_t indirect_addr = 0x2000;  // Where the target address is stored
        uint16_t target_addr = 0x3050;

        ram.write(cpu.regs.PC, JMP_IND);
        ram.write(cpu.regs.PC+1,0x00);
        ram.write(cpu.regs.PC+2,0x20);
        ram.write(indirect_addr, 0x50);
        ram.write(indirect_addr+1, 0x30);
        cpu.execute();

        REQUIRE(cpu.regs.PC == target_addr);
    }
}