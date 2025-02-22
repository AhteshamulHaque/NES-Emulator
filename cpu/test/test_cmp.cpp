#include "catch2/catch_all.hpp"

#include "cpu.h"
#include "ram.h"

#define CMP_IMM 0xC9

TEST_CASE("CMP Instruction", "[CMP]") {

    CPU6502 cpu;
    RAM ram;
    cpu.connect(ram);

    SECTION("A > Immediate Value") {
        cpu.regs.A = 0x30;
        cpu.regs.PC = 0x1000;

        ram.write(cpu.regs.PC, CMP_IMM);
        ram.write(cpu.regs.PC+1, 0x20);
        cpu.execute();

        REQUIRE(cpu.regs.flags.C == 1); // Carry flag set (no borrow)
        REQUIRE(cpu.regs.flags.Z == 0); // Not equal
        REQUIRE(cpu.regs.flags.N == 0); // Result not negative
        REQUIRE(cpu.regs.PC == 0x1002);
    }

    SECTION("A < Immediate Value") {
        cpu.regs.A = 0x20;
        cpu.regs.PC = 0x1000;
        uint8_t value = 0x30;

        ram.write(cpu.regs.PC, CMP_IMM);
        ram.write(cpu.regs.PC+1, 0x30);
        cpu.execute();

        REQUIRE(cpu.regs.flags.C == 0); // Carry flag clear (borrow)
        REQUIRE(cpu.regs.flags.Z == 0); // Not equal
        REQUIRE(cpu.regs.flags.N == 1); // Result is negative (two's complement borrow)
        REQUIRE(cpu.regs.PC == 0x1002);
    }

    SECTION("A = Immediate Value") {
        cpu.regs.A = 0x20;
        cpu.regs.PC = 0x1000;
        uint8_t value = 0x20;

        ram.write(cpu.regs.PC, CMP_IMM);
        ram.write(cpu.regs.PC+1, 0x20);
        cpu.execute();

        REQUIRE(cpu.regs.flags.C == 1); // Carry flag set (no borrow)
        REQUIRE(cpu.regs.flags.Z == 1); // Equal
        REQUIRE(cpu.regs.flags.N == 0); // Result not negative
        REQUIRE(cpu.regs.PC == 0x1002);
    }

    SECTION("A = $00, Immediate Value = $00") {
        cpu.regs.A = 0x00;
        cpu.regs.PC = 0x1000;

        ram.write(cpu.regs.PC, CMP_IMM);
        ram.write(cpu.regs.PC+1, 0x00);
        cpu.execute();

        REQUIRE(cpu.regs.flags.C == 1);
        REQUIRE(cpu.regs.flags.Z == 1);
        REQUIRE(cpu.regs.flags.N == 0);
        REQUIRE(cpu.regs.PC == 0x1002);
    }
}