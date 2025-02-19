#include "catch2/catch_all.hpp"
#include <stdio.h>

#include "cpu.h"
#include "ram.h"

#define ADC_IMM 0x69

TEST_CASE("ADC Test Cases", "[ADC]") {

    CPU6502 cpu;
    RAM ram;
    cpu.connect(ram);

    SECTION("Positive addition, no carry, no overflow") {
        cpu.regs.PC = 0x1000;
        cpu.regs.A = 0x10;
        cpu.regs.flags.C = 0;

        ram.write(cpu.regs.PC, ADC_IMM);
        ram.write(cpu.regs.PC+1, 0x20);
        cpu.execute();

        REQUIRE(cpu.regs.A == 0x30);
        REQUIRE(cpu.regs.PC == 0x1002);
        REQUIRE(cpu.regs.flags.N == 0);
        REQUIRE(cpu.regs.flags.Z == 0);
        REQUIRE(cpu.regs.flags.C == 0);
        REQUIRE(cpu.regs.flags.V == 0);
    }

    SECTION("Addition with Carry, no overflow") {
        cpu.regs.A = 0x10;
        cpu.regs.PC = 0x1000;
        cpu.regs.flags.C = 1;

        ram.write(cpu.regs.PC, ADC_IMM);
        ram.write(cpu.regs.PC+1, 0x20);
        cpu.execute();

        REQUIRE(cpu.regs.A == 0x31); // 0x10 + 0x20 + 0x01 (carry)
        REQUIRE(cpu.regs.PC == 0x1002);
        REQUIRE(cpu.regs.flags.N == 0);
        REQUIRE(cpu.regs.flags.Z == 0);
        REQUIRE(cpu.regs.flags.C == 0);
        REQUIRE(cpu.regs.flags.V == 0);
    }

    SECTION("Carry Set (Unsigned Overflow), No Signed Overflow") {
        cpu.regs.A = 0x80;
        cpu.regs.PC = 0x1000;
        cpu.regs.flags.C = 0;

        ram.write(cpu.regs.PC, ADC_IMM);
        ram.write(cpu.regs.PC+1, 0x80);
        cpu.execute();

        REQUIRE(cpu.regs.A == 0x00); // 0x80 + 0x80 = 0x100 (overflow, A becomes 0x00)
        REQUIRE(cpu.regs.PC == 0x1002);
        REQUIRE(cpu.regs.flags.N == 0);
        REQUIRE(cpu.regs.flags.Z == 1);
        REQUIRE(cpu.regs.flags.C == 1);
        REQUIRE(cpu.regs.flags.V == 1);
    }

    SECTION("Negative Result, No Overflow") {
        cpu.regs.A = 0x01;
        cpu.regs.PC = 0x1000;
        cpu.regs.flags.C = 0;

        ram.write(cpu.regs.PC, ADC_IMM);
        ram.write(cpu.regs.PC+1, 0xFF); // -1 in two's complement
        cpu.execute();

        REQUIRE(cpu.regs.A == 0x00); // 0x01 + 0xFF = 0x100 (overflow, A becomes 0x00)
        REQUIRE(cpu.regs.PC == 0x1002);
        REQUIRE(cpu.regs.flags.N == 0);
        REQUIRE(cpu.regs.flags.Z == 1);
        REQUIRE(cpu.regs.flags.C == 1);
        REQUIRE(cpu.regs.flags.V == 0);
    }

    SECTION("Positive Overflow") {
        cpu.regs.A = 0x7F; // 127 (max positive signed 8-bit value)
        cpu.regs.PC = 0x1000;
        cpu.regs.flags.C = 0;
        uint8_t immediateValue = 0x01;

        ram.write(cpu.regs.PC, ADC_IMM);
        ram.write(cpu.regs.PC+1, 0x01); // -1 in two's complement
        cpu.execute();

        REQUIRE(cpu.regs.A == 0x80); // -128 (overflow, A becomes negative)
        REQUIRE(cpu.regs.PC == 0x1002);
        REQUIRE(cpu.regs.flags.N == 1);
        REQUIRE(cpu.regs.flags.Z == 0);
        REQUIRE(cpu.regs.flags.C == 0);
        REQUIRE(cpu.regs.flags.V == 1);
    }
}