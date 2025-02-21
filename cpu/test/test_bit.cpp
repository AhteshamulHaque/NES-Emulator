#include "catch2/catch_all.hpp"

#include "cpu.h"
#include "ram.h"

#define BIT_ZPG 0x24

TEST_CASE("BIT Instruction", "[BIT]") {


    CPU6502 cpu;
    RAM ram;
    cpu.connect(ram);

    SECTION("Matching Bits") {
        cpu.regs.PC = 0x1000;
        cpu.regs.A = 0x05; // 0000 0101
        uint8_t zero_page_address = 0x20;

        ram.write(cpu.regs.PC, BIT_ZPG);
        ram.write(cpu.regs.PC+1, zero_page_address);
        ram.write(zero_page_address, 0x05); // 0000 0101
        cpu.execute();

        REQUIRE(cpu.regs.PC == 0x1002);
        REQUIRE(cpu.regs.flags.N == 0); // Bit 7 of memory is 0
        REQUIRE(cpu.regs.flags.V == 0); // Bit 6 of memory is 0
        REQUIRE(cpu.regs.flags.Z == 0); // A & Memory != 0
    }

    SECTION("No Matching Bits") {
        cpu.regs.PC = 0x1000;
        cpu.regs.A = 0x0A; // 0000 1010
        uint8_t zero_page_address = 0x30;

        ram.write(cpu.regs.PC, BIT_ZPG);
        ram.write(cpu.regs.PC+1, zero_page_address);
        ram.write(zero_page_address, 0x05); // 0000 0101
        cpu.execute();

        REQUIRE(cpu.regs.PC == 0x1002);
        REQUIRE(cpu.regs.flags.N == 0); // Bit 7 of memory is 0
        REQUIRE(cpu.regs.flags.V == 0); // Bit 6 of memory is 0
        REQUIRE(cpu.regs.flags.Z == 1); // A & Memory == 0
    }

    SECTION("N Flag Set") {
        cpu.regs.PC = 0x1000;
        cpu.regs.A = 0x00;
        uint8_t zero_page_address = 0x40;

        ram.write(cpu.regs.PC, BIT_ZPG);
        ram.write(cpu.regs.PC+1, zero_page_address);
        ram.write(zero_page_address, 0x80); // 1000 0000
        cpu.execute();

        REQUIRE(cpu.regs.PC == 0x1002);
        REQUIRE(cpu.regs.flags.N == 1); // Bit 7 of memory is 1
        REQUIRE(cpu.regs.flags.V == 0); // Bit 6 of memory is 0
        REQUIRE(cpu.regs.flags.Z == 1); // A & Memory == 0
    }

    SECTION("V Flag Set") {
        cpu.regs.PC = 0x1000;
        cpu.regs.A = 0x00;
        uint8_t zero_page_address = 0x50;

        ram.write(cpu.regs.PC, BIT_ZPG);
        ram.write(cpu.regs.PC+1, zero_page_address);
        ram.write(zero_page_address, 0x40); // 0100 0000
        cpu.execute();

        REQUIRE(cpu.regs.PC == 0x1002);
        REQUIRE(cpu.regs.flags.N == 0); // Bit 7 of memory is 0
        REQUIRE(cpu.regs.flags.V == 1); // Bit 6 of memory is 1
        REQUIRE(cpu.regs.flags.Z == 1); // A & Memory == 0
    }

    SECTION("All Bits Match (A != 0)") {
        cpu.regs.PC = 0x1000;
        cpu.regs.A = 0xFF; // 1111 1111
        uint8_t zero_page_address = 0x60;

        ram.write(cpu.regs.PC, BIT_ZPG);
        ram.write(cpu.regs.PC+1, zero_page_address);
        ram.write(zero_page_address, 0xFF); // 1111 1111
        cpu.execute();

        REQUIRE(cpu.regs.PC == 0x1002);
        REQUIRE(cpu.regs.flags.N == 1); // Bit 7 of memory is 1
        REQUIRE(cpu.regs.flags.V == 1); // Bit 6 of memory is 1
        REQUIRE(cpu.regs.flags.Z == 0); // A & Memory != 0
    }

    SECTION("All Bits Match (A = 0)") {
        cpu.regs.PC = 0x1000;
        cpu.regs.A = 0x00;
        uint8_t zero_page_address = 0x70;

        ram.write(cpu.regs.PC, BIT_ZPG);
        ram.write(cpu.regs.PC+1, zero_page_address);
        ram.write(zero_page_address, 0xFF); // 1111 1111
        cpu.execute();

        REQUIRE(cpu.regs.PC == 0x1002);
        REQUIRE(cpu.regs.flags.N == 1); // Bit 7 of memory is 1
        REQUIRE(cpu.regs.flags.V == 1); // Bit 6 of memory is 1
        REQUIRE(cpu.regs.flags.Z == 1); // A & Memory == 0
    }
}