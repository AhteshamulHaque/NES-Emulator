#include "catch2/catch_all.hpp"

#include "cpu.h"
#include "ram.h"

#define AND_IMM 0x29

TEST_CASE("AND Instruction", "[AND]") {

    CPU6502 cpu;
    RAM ram;
    cpu.connect(ram);

    SECTION("Same bits set") {
        cpu.regs.A = 0x0F; // 0000 1111
        cpu.regs.PC = 0x1000;

        ram.write(cpu.regs.PC, AND_IMM);
        ram.write(cpu.regs.PC+1, 0x03); // 0000 0011
        cpu.execute();

        REQUIRE(cpu.regs.A == 0x03); // 0000 0011
        REQUIRE(cpu.regs.PC == 0x1002);
        REQUIRE(cpu.regs.flags.N == 0);
        REQUIRE(cpu.regs.flags.Z == 0);
    }

    SECTION("Same bit different") {
        cpu.regs.A = 0x05; // 0000 0101
        cpu.regs.PC = 0x1000;

        ram.write(cpu.regs.PC, AND_IMM);
        ram.write(cpu.regs.PC+1, 0x02); // 0000 0010
        cpu.execute();

        REQUIRE(cpu.regs.A == 0x00); // 0000 0000
        REQUIRE(cpu.regs.PC == 0x1002);
        REQUIRE(cpu.regs.flags.Z == 1);
        REQUIRE(cpu.regs.flags.N == 0);
    }

    SECTION("Negative Result (bit 7 set)") {
        cpu.regs.A = 0xF0; // 1111 0000
        cpu.regs.PC = 0x1000;

        ram.write(cpu.regs.PC, AND_IMM);
        ram.write(cpu.regs.PC+1, 0x80); // 1000 0000
        cpu.execute();

        REQUIRE(cpu.regs.A == 0x80); // 1000 0000
        REQUIRE(cpu.regs.PC == 0x1002);
        REQUIRE(cpu.regs.flags.N == 1); // Negative flag set
        REQUIRE(cpu.regs.flags.Z == 0);
    }
}