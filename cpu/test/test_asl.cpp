#include "catch2/catch_all.hpp"

#include "cpu.h"
#include "ram.h"

#define ASL_ACC 0x0A
#define ASL_ZPG 0x06
#define ASL_XZP 0x16
#define ASL_ABS 0x0E
#define ASL_XAB 0x1E

TEST_CASE("ASL Accumulator Addressing Mode", "[ASL]") {

    CPU6502 cpu;
    RAM ram;
    cpu.connect(ram);

    SECTION("Accumulator contains a positive value, no carry") {
        cpu.regs.PC = 0x1000;
        cpu.regs.A = 0x20;

        ram.write(cpu.regs.PC, ASL_ACC);
        cpu.execute();

        REQUIRE(cpu.regs.A == 0x40);
        REQUIRE(cpu.regs.flags.N == 0);
        REQUIRE(cpu.regs.flags.Z == 0);
        REQUIRE(cpu.regs.flags.C == 0);
    }

    SECTION("Accumulator contains a positive value, carry set") {
        cpu.regs.PC = 0x1000;
        cpu.regs.A = 0x40;

        ram.write(cpu.regs.PC, ASL_ACC);
        cpu.execute();

        REQUIRE(cpu.regs.A == 0x80);
        REQUIRE(cpu.regs.flags.N == 1);
        REQUIRE(cpu.regs.flags.Z == 0);
        REQUIRE(cpu.regs.flags.C == 0);
    }

    SECTION("Accumulator contains a zero value") {
        cpu.regs.PC = 0x1000;
        cpu.regs.A = 0x00;

        ram.write(cpu.regs.PC, ASL_ACC);
        cpu.execute();

        REQUIRE(cpu.regs.A == 0x00);
        REQUIRE(cpu.regs.flags.N == 0);
        REQUIRE(cpu.regs.flags.Z == 1);
        REQUIRE(cpu.regs.flags.C == 0);
    }

    SECTION("Accumulator contains a positive value, carry set, no overflow") {
        cpu.regs.PC = 0x1000;
        cpu.regs.A = 0x80;

        ram.write(cpu.regs.PC, ASL_ACC);
        cpu.execute();

        REQUIRE(cpu.regs.A == 0x00);
        REQUIRE(cpu.regs.flags.N == 0);
        REQUIRE(cpu.regs.flags.Z == 1);
        REQUIRE(cpu.regs.flags.C == 1);
    }

    SECTION("Accumulator contains a positive value, carry set, overflow") {
        cpu.regs.PC = 0x1000;
        cpu.regs.A = 0xC0;

        ram.write(cpu.regs.PC, ASL_ACC);
        cpu.execute();

        REQUIRE(cpu.regs.A == 0x80);
        REQUIRE(cpu.regs.flags.N == 1);
        REQUIRE(cpu.regs.flags.Z == 0);
        REQUIRE(cpu.regs.flags.C == 1);
    }
}

TEST_CASE("ASL Absolute Addressing Mode", "[ASL]") {

    CPU6502 cpu;
    RAM ram;
    cpu.connect(ram);

    SECTION("Value at address $nnnn is positive") {
        cpu.regs.PC = 0x1000;
        uint16_t target_address = 0x2050;

        ram.write(cpu.regs.PC, ASL_ABS);
        ram.write(cpu.regs.PC+1, 0x50);
        ram.write(cpu.regs.PC+2, 0x20);
        ram.write(target_address, 0x20);
        cpu.execute();

        REQUIRE(ram.read(target_address) == 0x40);
        REQUIRE(cpu.regs.PC == 0x1003);
        REQUIRE(cpu.regs.flags.N == 0);
        REQUIRE(cpu.regs.flags.Z == 0);
        REQUIRE(cpu.regs.flags.C == 0);
    }

    SECTION("Value at address $nnnn sets carry") {
        cpu.regs.PC = 0x1000;
        uint16_t target_address = 0x30A0;

        ram.write(cpu.regs.PC, ASL_ABS);
        ram.write(cpu.regs.PC+1, 0xA0);
        ram.write(cpu.regs.PC+2, 0x30);
        ram.write(target_address, 0x80);
        cpu.execute();

        REQUIRE(ram.read(target_address) == 0x00);
        REQUIRE(cpu.regs.PC == 0x1003);
        REQUIRE(cpu.regs.flags.N == 0);
        REQUIRE(cpu.regs.flags.Z == 1);
        REQUIRE(cpu.regs.flags.C == 1);
    }

    SECTION("Value at address $nnnn sets negative flag") {
        cpu.regs.PC = 0x1000;
        uint16_t target_address = 0x4000;
        ram.write(cpu.regs.PC, ASL_ABS);
        ram.write(cpu.regs.PC+1, 0x00);
        ram.write(cpu.regs.PC+2, 0x40);
        ram.write(target_address, 0x7F);
        cpu.execute();

        REQUIRE(ram.read(target_address) == 0xFE);
        REQUIRE(cpu.regs.PC == 0x1003);
        REQUIRE(cpu.regs.flags.N == 1);
        REQUIRE(cpu.regs.flags.Z == 0);
        REQUIRE(cpu.regs.flags.C == 0);
    }
}

TEST_CASE("ASL Zero Page Addressing Mode", "[ASL]") {

    CPU6502 cpu;
    RAM ram;

    cpu.connect(ram);

    SECTION("When result is between $00 and $7F") {
        cpu.regs.PC = 0x1000;
        uint8_t zero_page_address = 0x20;

        ram.write(cpu.regs.PC, ASL_ZPG);
        ram.write(cpu.regs.PC+1, zero_page_address);
        ram.write(zero_page_address, 0x10);
        cpu.execute();

        REQUIRE(ram.read(zero_page_address) == 0x20);
        REQUIRE(cpu.regs.PC == 0x1002);
        REQUIRE(cpu.regs.flags.N == 0);
        REQUIRE(cpu.regs.flags.Z == 0);
        REQUIRE(cpu.regs.flags.C == 0);
    }

    SECTION("When result goes beyond $FF") {
        cpu.regs.PC = 0x1000;
        uint8_t zero_page_address = 0x30;

        ram.write(cpu.regs.PC, ASL_ZPG);
        ram.write(cpu.regs.PC+1, zero_page_address);
        ram.write(zero_page_address, 0x80);
        cpu.execute();

        REQUIRE(ram.read(zero_page_address) == 0x00);
        REQUIRE(cpu.regs.PC == 0x1002);
        REQUIRE(cpu.regs.flags.N == 0);
        REQUIRE(cpu.regs.flags.Z == 1);
        REQUIRE(cpu.regs.flags.C == 1);
    }

    SECTION("When the result is between $80 and $FF") {
        cpu.regs.PC = 0x1000;
        uint8_t zero_page_address = 0x40;
        ram.write(cpu.regs.PC, ASL_ZPG);
        ram.write(cpu.regs.PC+1, zero_page_address);
        ram.write(zero_page_address, 0x7F);
        cpu.execute();

        REQUIRE(ram.read(zero_page_address) == 0xFE);
        REQUIRE(cpu.regs.PC == 0x1002);
        REQUIRE(cpu.regs.flags.N == 1);
        REQUIRE(cpu.regs.flags.Z == 0);
        REQUIRE(cpu.regs.flags.C == 0);
    }
}

TEST_CASE("ASL X-Indexed Zero Page Addressing Mode", "[ASL]") {

    CPU6502 cpu;
    RAM ram;
    cpu.connect(ram);

    SECTION("When target address is below $FF") {
        cpu.regs.PC = 0x1000;
        cpu.regs.X = 0x05;
        uint8_t base_address = 0x20;
        uint8_t target_address = base_address + cpu.regs.X; // 0x25

        ram.write(cpu.regs.PC, ASL_XZP);
        ram.write(cpu.regs.PC+1, base_address);
        ram.write(target_address, 0x7F);
        cpu.execute();

        REQUIRE(ram.read(target_address) == 0xFE);
        REQUIRE(cpu.regs.PC == 0x1002);
        REQUIRE(cpu.regs.flags.N == 1);
        REQUIRE(cpu.regs.flags.Z == 0);
        REQUIRE(cpu.regs.flags.C == 0);
    }

    SECTION("When value is more than $FF, carry set") {
        cpu.regs.PC = 0x1000;
        cpu.regs.X = 0x0A;
        uint8_t base_address = 0x30;
        uint8_t target_address = base_address + cpu.regs.X;

        ram.write(cpu.regs.PC, ASL_XZP);
        ram.write(cpu.regs.PC+1, base_address);
        ram.write(target_address, 0x80);
        cpu.execute();

        REQUIRE(ram.read(target_address) == 0x00);
        REQUIRE(cpu.regs.PC == 0x1002);
        REQUIRE(cpu.regs.flags.N == 0);
        REQUIRE(cpu.regs.flags.Z == 1);
        REQUIRE(cpu.regs.flags.C == 1);
    }

    SECTION("Zero Page Wrap-Around") {
        cpu.regs.PC = 0x1000;
        cpu.regs.X = 0x01;
        uint8_t base_address = 0xFF; // Wraps to 0x00
        uint8_t target_address = base_address + cpu.regs.X; // Calculate wrapped address

        ram.write(cpu.regs.PC, ASL_XZP);
        ram.write(cpu.regs.PC+1, base_address);
        ram.write(target_address, 0x40);
        cpu.execute();

        REQUIRE(ram.read(target_address) == 0x80);
        REQUIRE(cpu.regs.PC == 0x1002);
        REQUIRE(cpu.regs.flags.N == 1);
        REQUIRE(cpu.regs.flags.Z == 0);
        REQUIRE(cpu.regs.flags.C == 0);
    }
}