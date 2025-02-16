#include "catch2/catch_all.hpp"
#include "cpu.h"
#include "ram.h"

#define LDA_IMM 0xA9
#define LDA_ZPG 0xA5
#define LDA_XZP 0xB5
#define LDA_XZI 0xA1
#define LDA_ABS 0xAD
#define LDA_XAB 0xBD
#define LDA_YAB 0xB9
#define LDA_ZIY 0xB1

/*
    Test cases covered:
        1. Load value from $00 - $FF
        2. Load value $00
        3. Load value $FF
        4. Load value greater than $FF
*/
TEST_CASE("LDA Immediate Addressing Mode", "[6502]") {

    CPU6502 cpu;
    RAM ram;
    cpu.connect(ram);

    SECTION("Loading value from $00 - $FF") {
        cpu.regs.PC = 0x1000;
        ram.write(cpu.regs.PC, LDA_IMM);
        ram.write(cpu.regs.PC+1, 0x20);
        cpu.execute();

        REQUIRE(cpu.regs.A == 0x20);
        REQUIRE(cpu.regs.PC == 0x1002);
        REQUIRE(cpu.regs.flags.Z == 0);
        REQUIRE(cpu.regs.flags.N == 0);
    }

    SECTION("Loading zero value ($00)") {
        cpu.regs.PC = 0x1000;
        ram.write(cpu.regs.PC, LDA_IMM);
        ram.write(cpu.regs.PC+1, 0x0);
        cpu.execute();

        REQUIRE(cpu.regs.A == 0x0);
        REQUIRE(cpu.regs.PC == 0x1002);
        REQUIRE(cpu.regs.flags.Z == 1);
        REQUIRE(cpu.regs.flags.N == 0);
    }

    SECTION("Loading max value ($FF)") {
        cpu.regs.PC = 0x1000;
        ram.write(cpu.regs.PC, LDA_IMM);
        ram.write(cpu.regs.PC+1, 0xFF);
        cpu.execute();

        REQUIRE(cpu.regs.A == 0xFF);
        REQUIRE(cpu.regs.PC == 0x1002);
        REQUIRE(cpu.regs.flags.Z == 0);
        REQUIRE(cpu.regs.flags.N == 1);
    }

    SECTION("Loading value beyond $FF") {
        cpu.regs.PC = 0x1000;
        ram.write(cpu.regs.PC, LDA_IMM);
        ram.write(cpu.regs.PC+1, (uint8_t)0x105);
        cpu.execute();

        REQUIRE(cpu.regs.A == 0x05);
        REQUIRE(cpu.regs.PC == 0x1002);
        REQUIRE(cpu.regs.flags.Z == 0);
        REQUIRE(cpu.regs.flags.N == 0);
    }
}

/*
Test cases coverd:
    1. Load value from address range 0x00 - 0xFF
    2. Load value from memory 0x00
    3. Load value from memory 0xFF
*/
TEST_CASE("LDA Zero Page Addressing Mode", "[6502]") {

    CPU cpu;
    RAM ram;
    cpu.connect(ram);

    SECTION("Load value from address range 0x00 - 0xFF") {
        cpu.regs.PC = 0x1000;
        uint8_t zero_page_address = 0x20;
        ram.write(cpu.regs.PC, LDA_ZPG);
        ram.write(cpu.regs.PC+1, zero_page_address);
        ram.write(zero_page_address, 0x42);
        cpu.execute();

        REQUIRE(cpu.regs.A == 0x42);
        REQUIRE(cpu.regs.PC == 0x1002);
        REQUIRE(cpu.regs.flags.Z == 0);
        REQUIRE(cpu.regs.flags.N == 0);
    }

    SECTION("Load value from address 0x00") {
        cpu.regs.PC = 0x1000;
        uint8_t zero_page_address = 0x00;
        ram.write(cpu.regs.PC, LDA_ZPG);
        ram.write(cpu.regs.PC+1, zero_page_address);
        ram.write(zero_page_address, 0xFF);
        cpu.execute();

        REQUIRE(cpu.regs.A == 0xFF);
        REQUIRE(cpu.regs.PC == 0x1002);
        REQUIRE(cpu.regs.flags.Z == 0);
        REQUIRE(cpu.regs.flags.N == 1);
    }

    SECTION("Load value from address 0xFF") {
        PC = 0x1000;
        uint8_t zero_page_address = 0xFF;
        ram.write(cpu.regs.PC, LDA_ZPG);
        ram.write(cpu.regs.PC+1, zero_page_address);
        ram.write(zero_page_address, 0x7F);
        cpu.execute();

        REQUIRE(cpu.regs.A == 0x7F);
        REQUIRE(cpu.regs.PC == 0x1002);
        REQUIRE(cpu.regs.flags.Z == 0);
        REQUIRE(cpu.regs.flags.N == 0);
    }
}