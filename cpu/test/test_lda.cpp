#include "catch2/catch_all.hpp"
#include <stdio.h>

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

TEST_CASE("LDA Immediate Addressing Mode", "[LDA]") {

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

TEST_CASE("LDA Zero Page Addressing Mode", "[LDA]") {

    CPU6502 cpu;
    RAM ram;
    cpu.connect(ram);

    SECTION("Load value from address range $00 - $FF") {
        cpu.regs.PC = 0x1000;
        uint8_t zero_page_addr = 0x20;

        ram.write(cpu.regs.PC, LDA_ZPG);
        ram.write(cpu.regs.PC+1, zero_page_addr);
        ram.write(zero_page_addr, 0x42);
        cpu.execute();

        REQUIRE(cpu.regs.A == 0x42);
        REQUIRE(cpu.regs.PC == 0x1002);
        REQUIRE(cpu.regs.flags.Z == 0);
        REQUIRE(cpu.regs.flags.N == 0);
    }

    SECTION("Load value from address $00") {
        cpu.regs.PC = 0x1000;
        uint8_t zero_page_addr = 0x00;

        ram.write(cpu.regs.PC, LDA_ZPG);
        ram.write(cpu.regs.PC+1, zero_page_addr);
        ram.write(zero_page_addr, 0xFF);
        cpu.execute();

        REQUIRE(cpu.regs.A == 0xFF);
        REQUIRE(cpu.regs.PC == 0x1002);
        REQUIRE(cpu.regs.flags.Z == 0);
        REQUIRE(cpu.regs.flags.N == 1);
    }

    SECTION("Load value from address $FF") {
        cpu.regs.PC = 0x1000;
        uint8_t zero_page_addr = 0xFF;

        ram.write(cpu.regs.PC, LDA_ZPG);
        ram.write(cpu.regs.PC+1, zero_page_addr);
        ram.write(zero_page_addr, 0x7F);
        cpu.execute();

        REQUIRE(cpu.regs.A == 0x7F);
        REQUIRE(cpu.regs.PC == 0x1002);
        REQUIRE(cpu.regs.flags.Z == 0);
        REQUIRE(cpu.regs.flags.N == 0);
    }
}

TEST_CASE("LDA Zero Page X-Indexed Addressing Mode", "[LDA]") {
    CPU6502 cpu;
    RAM ram;
    cpu.connect(ram);

    SECTION("Target address between $00 - $FF") {
        cpu.regs.X = 0x10;
        cpu.regs.PC = 0x1000;
        uint8_t zero_page_addr = 0x05;
        uint8_t target_addr = zero_page_addr + cpu.regs.X;

        ram.write(cpu.regs.PC, LDA_XZP);
        ram.write(cpu.regs.PC+1, zero_page_addr);
        ram.write(target_addr, 0x2A);
        cpu.execute();

        REQUIRE(cpu.regs.A == 0x2A);
        REQUIRE(cpu.regs.PC == 0x1002);
        REQUIRE(cpu.regs.flags.Z == 0);
        REQUIRE(cpu.regs.flags.N == 0);
    }

    SECTION("Target address beyond $FF (Wraparound)") {
        cpu.regs.X = 0xFF; // Wraps to 0xFE
        cpu.regs.PC = 0x1000;
        uint8_t zero_page_addr = 0xFF;
        uint8_t target_addr = zero_page_addr + cpu.regs.X;  // Wraps to 0xFD

        ram.write(cpu.regs.PC, LDA_XZP);
        ram.write(cpu.regs.PC+1, zero_page_addr);
        ram.write(target_addr, 0xCC);
        cpu.execute();

        REQUIRE(cpu.regs.A == 0xCC);
        REQUIRE(cpu.regs.PC == 0x1002);
        REQUIRE(cpu.regs.flags.Z == 0);
        REQUIRE(cpu.regs.flags.N == 1);
    }
}

TEST_CASE("LDA X-Indexed Zero Page Indirect Addressing Mode", "[LDA]") {
    CPU6502 cpu;
    RAM ram;
    cpu.connect(ram);

    SECTION("Target address between address range $00 - $FE") {
        cpu.regs.X = 0x05;
        cpu.regs.PC = 0x1000;
        uint8_t zero_page_addr = 0x10;
        uint8_t target_addr = zero_page_addr + cpu.regs.X;

        ram.write(cpu.regs.PC, LDA_XZI);
        ram.write(cpu.regs.PC+1, zero_page_addr);
        ram.write(target_addr, 0xAB);
        ram.write(target_addr+1, 0x12);
        ram.write(0x12AB, 0x12);
        cpu.execute();

        REQUIRE(cpu.regs.A == 0x12);
        REQUIRE(cpu.regs.PC == 0x1002);
        REQUIRE(cpu.regs.flags.Z == 0);
        REQUIRE(cpu.regs.flags.N == 0);
    }

    SECTION("Target address at $FF") {
        cpu.regs.X = 0x64;  // $64 + $9B = $FF
        cpu.regs.PC = 0x1000;
        uint8_t zero_page_addr = 0x9B;
        uint8_t target_addr = zero_page_addr + cpu.regs.X;

        ram.write(cpu.regs.PC, LDA_XZI);
        ram.write(cpu.regs.PC+1, zero_page_addr);
        ram.write(target_addr, 0x4B);
        ram.write((uint8_t)(target_addr+1), 0xA9);
        ram.write(0xA94B, 0x90);
        cpu.execute();

        REQUIRE(cpu.regs.A == 0x90);
        REQUIRE(cpu.regs.PC == 0x1002);
        REQUIRE(cpu.regs.flags.Z == 0);
        REQUIRE(cpu.regs.flags.N == 1);
    }
}

TEST_CASE("LDA Absolute Addressing Mode", "[LDA]") {
    CPU6502 cpu;
    RAM ram;
    cpu.connect(ram);

    SECTION("Load a positive value from address $nnnn") {
        cpu.regs.PC = 0x1000;
        uint16_t base_addr = 0x2050; // any address

        ram.write(cpu.regs.PC, LDA_ABS);
        ram.write(cpu.regs.PC+1, 0x50);
        ram.write(cpu.regs.PC+2, 0x20);
        ram.write(base_addr, 0x42);
        cpu.execute();

        REQUIRE(cpu.regs.A == 0x42);
        REQUIRE(cpu.regs.PC == 0x1003);
        REQUIRE(cpu.regs.flags.Z == 0);
        REQUIRE(cpu.regs.flags.N == 0);
    }

    SECTION("Load a negative value from address $nnnn") {
        cpu.regs.PC = 0x1000;
        uint16_t base_addr = 0x30A0;  // any address

        ram.write(cpu.regs.PC, LDA_ABS);
        ram.write(cpu.regs.PC+1, 0xA0);
        ram.write(cpu.regs.PC+2, 0x30);
        ram.write(base_addr, 0xED);
        cpu.execute();

        REQUIRE(cpu.regs.A == 0xED);
        REQUIRE(cpu.regs.PC == 0x1003);
        REQUIRE(cpu.regs.flags.Z == 0);
        REQUIRE(cpu.regs.flags.N == 1);
    }
}

TEST_CASE("LDA X-Indexed Absolute Addressing Mode", "[LDA]") {
    CPU6502 cpu;
    RAM ram;
    cpu.connect(ram);

    SECTION("Load a value within same page $nnnn + X") {
        cpu.regs.X = 0x05;
        cpu.regs.PC = 0x1000;
        uint16_t base_addr = 0x9AE5; // any address

        ram.write(cpu.regs.PC, LDA_XAB);
        ram.write(cpu.regs.PC+1, 0xE5);
        ram.write(cpu.regs.PC+2, 0x9A);
        ram.write(base_addr+cpu.regs.X, 0x42);
        cpu.execute();

        REQUIRE(cpu.regs.A == 0x42);
        REQUIRE(cpu.regs.PC == 0x1003);
        REQUIRE(cpu.regs.flags.Z == 0);
        REQUIRE(cpu.regs.flags.N == 0);
    }

    // This test makes sense for cycle testing
    SECTION("Load a value from different page $nnnn + $FF") {
        cpu.regs.X = 0xFF;
        cpu.regs.PC = 0x1000;
        uint16_t base_addr = 0x3AE6;

        ram.write(cpu.regs.PC, LDA_XAB);
        ram.write(cpu.regs.PC+1, 0xE6);
        ram.write(cpu.regs.PC+2, 0x3A);
        ram.write(base_addr+cpu.regs.X, 0x88);
        cpu.execute();

        REQUIRE(cpu.regs.A == 0x88);
        REQUIRE(cpu.regs.PC == 0x1003);
        REQUIRE(cpu.regs.flags.Z == 0);
        REQUIRE(cpu.regs.flags.N == 1);
    }

    SECTION("Load value by wrapping around $FFFF") {
        cpu.regs.X = 0xA;
        cpu.regs.PC = 0x1000;
        uint16_t base_addr = 0xFFFB;

        ram.write(cpu.regs.PC, LDA_XAB);
        ram.write(cpu.regs.PC+1, 0xFB);
        ram.write(cpu.regs.PC+2, 0xFF);
        ram.write(base_addr+cpu.regs.X, 0x00);
        cpu.execute();

        REQUIRE(cpu.regs.A == 0x00);
        REQUIRE(cpu.regs.PC == 0x1003);
        REQUIRE(cpu.regs.flags.Z == 1);
        REQUIRE(cpu.regs.flags.N == 0);
    }
}

TEST_CASE("LDA Y-Indexed Absolute Addressing Mode", "[LDA]") {
    CPU6502 cpu;
    RAM ram;
    cpu.connect(ram);

    SECTION("Load a value within same page $nnnn + X") {
        cpu.regs.Y = 0x15;
        cpu.regs.PC = 0x1000;
        uint16_t base_addr = 0x44A5; // any address base_addr + cpu.regs.X;

        ram.write(cpu.regs.PC, LDA_YAB);
        ram.write(cpu.regs.PC+1, 0xA5);
        ram.write(cpu.regs.PC+2, 0x44);
        ram.write(base_addr+cpu.regs.Y, 0x27);
        cpu.execute();

        REQUIRE(cpu.regs.A == 0x27);
        REQUIRE(cpu.regs.PC == 0x1003);
        REQUIRE(cpu.regs.flags.Z == 0);
        REQUIRE(cpu.regs.flags.N == 0);
    }

    // This test makes sense for cycle testing
    SECTION("Load a value from different page $nnnn + $FF") {
        cpu.regs.Y = 0xFF;
        cpu.regs.PC = 0x1000;
        uint16_t base_addr = 0xDB58;

        ram.write(cpu.regs.PC, LDA_YAB);
        ram.write(cpu.regs.PC+1, 0x58);
        ram.write(cpu.regs.PC+2, 0xDB);
        ram.write(base_addr+cpu.regs.Y, 0xBE);
        cpu.execute();

        REQUIRE(cpu.regs.A == 0xBE);
        REQUIRE(cpu.regs.PC == 0x1003);
        REQUIRE(cpu.regs.flags.Z == 0);
        REQUIRE(cpu.regs.flags.N == 1);
    }

    SECTION("Load value by wrapping around $FFFF") {
        cpu.regs.Y = 0x09;
        cpu.regs.PC = 0x1000;
        uint16_t base_addr = 0xFFFE;
        ram.write(cpu.regs.PC, LDA_YAB);
        ram.write(cpu.regs.PC+1, 0xFE);
        ram.write(cpu.regs.PC+2, 0xFF);
        ram.write(base_addr+cpu.regs.Y, 0x45);
        cpu.execute();

        REQUIRE(cpu.regs.A == 0x45);
        REQUIRE(cpu.regs.PC == 0x1003);
        REQUIRE(cpu.regs.flags.Z == 0);
        REQUIRE(cpu.regs.flags.N == 0);
    }
}

TEST_CASE("LDA Zero Page Indirect Y-Indexed Addressing Mode", "[LDA]") {
    CPU6502 cpu;
    RAM ram;
    cpu.connect(ram);

    SECTION("Load a value from range $00 - $FF") {
        cpu.regs.Y = 0x05;
        cpu.regs.PC = 0x1000;
        uint8_t base_addr = 0xA5;
        uint16_t target_addr = 0xC678;

        ram.write(cpu.regs.PC, LDA_ZIY);
        ram.write(cpu.regs.PC+1, base_addr);
        ram.write(base_addr, 0x78);
        ram.write(base_addr+1, 0xC6);
        ram.write(target_addr+cpu.regs.Y, 0x38);
        cpu.execute();

        REQUIRE(cpu.regs.A == 0x38);
        REQUIRE(cpu.regs.PC == 0x1002);
        REQUIRE(cpu.regs.flags.N == 0);
        REQUIRE(cpu.regs.flags.Z == 0);
    }

    SECTION("Load a value from address at $FF") {
        cpu.regs.Y = 0x05;
        cpu.regs.PC = 0x1000;
        uint8_t base_addr = 0xFF;
        uint16_t target_addr = 0x58E8;

        ram.write(cpu.regs.PC, LDA_ZIY);
        ram.write(cpu.regs.PC+1, base_addr);
        ram.write(base_addr, 0xE8);
        ram.write((uint8_t)(base_addr+1), 0x58);
        ram.write(target_addr+cpu.regs.Y, 0xAC);
        cpu.execute();

        REQUIRE(cpu.regs.A == 0xAC);
        REQUIRE(cpu.regs.PC == 0x1002);
        REQUIRE(cpu.regs.flags.Z == 0);
        REQUIRE(cpu.regs.flags.N == 1);
    }

    // This test case only makes sense if testing cycle count
    SECTION("Load a value from different page") {
        cpu.regs.Y = 0xFF;
        cpu.regs.PC = 0x1000;
        uint8_t base_addr = 0x30;
        uint16_t target_addr = 0x21A3;

        ram.write(cpu.regs.PC, LDA_ZIY);
        ram.write(cpu.regs.PC+1, base_addr);
        ram.write(base_addr, 0xA3);
        ram.write((uint8_t)(base_addr+1), 0x21);
        ram.write(target_addr+cpu.regs.Y, 0xD8);
        cpu.execute();

        REQUIRE(cpu.regs.A == 0xD8);
        REQUIRE(cpu.regs.PC == 0x1002);
        REQUIRE(cpu.regs.flags.Z == 0);
        REQUIRE(cpu.regs.flags.N == 1);
    }
}