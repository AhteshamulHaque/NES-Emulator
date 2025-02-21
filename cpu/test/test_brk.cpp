#include "catch2/catch_all.hpp"

#include "cpu.h"
#include "ram.h"

#define BRK_IMP 0x00

TEST_CASE("BRK Instruction", "[BRK]") {

    CPU6502 cpu;
    RAM ram;
    cpu.connect(ram);

    SECTION("Interrupt Vector Set") {
        cpu.regs.PC = 0x1000;
        cpu.regs.flags.P = 0x20; // Initial flags (Break flag should be clear)
        cpu.regs.SP = 0xFF; // Initial stack pointer
        uint16_t irq_vector = 0x2000;

        ram.write(cpu.regs.PC, BRK_IMP);
        ram.write(cpu.regs.PC+1, 0xAA);  // Some value after the BRK instruction
        ram.write(0xFFFE, 0x00);      // Set low byte of interrupt vector
        ram.write(0xFFFF, 0x20); // Set high byte of interrupt vector
        cpu.execute();

        REQUIRE(cpu.regs.SP == 0xFC); // Stack pointer decremented by 3
        REQUIRE(cpu.regs.PC == irq_vector); // PC set to the interrupt vector
        REQUIRE(cpu.regs.flags.P == 0x24); // Interrupt flag set
        REQUIRE(ram.read(STACK_BASE_ADDRESS + (uint8_t)(cpu.regs.SP+1)) == 0x30);   // P pushed to stack with B flag set
        REQUIRE(ram.read(STACK_BASE_ADDRESS + (uint8_t)(cpu.regs.SP+2)) == 0x02);   // low byte PC+2 - 0x1002
        REQUIRE(ram.read(STACK_BASE_ADDRESS + (uint8_t)(cpu.regs.SP+3)) == 0x10);   // high byte PC+2
    }

    SECTION("Interrupts Disabled") {
        cpu.regs.PC = 0x1000;
        cpu.regs.flags.P = 0x24; // Interrupt disable flag set
        cpu.regs.SP = 0xFF;
        uint16_t irq_vector = 0x2000;

        ram.write(cpu.regs.PC, BRK_IMP);
        ram.write(cpu.regs.PC+1, 0xAA);
        ram.write(0xFFFE, 0x00);
        ram.write(0xFFFF, 0x20);
        cpu.execute();

        REQUIRE(cpu.regs.SP == 0xFC);
        REQUIRE(cpu.regs.PC == irq_vector);
        REQUIRE(cpu.regs.flags.P == 0x24); // Interrupt flag set
        REQUIRE(ram.read(STACK_BASE_ADDRESS+(uint8_t)(cpu.regs.SP+1)) == 0x34);
        REQUIRE(ram.read(STACK_BASE_ADDRESS+(uint8_t)(cpu.regs.SP+2)) == 0x02);
        REQUIRE(ram.read(STACK_BASE_ADDRESS+(uint8_t)(cpu.regs.SP+3)) == 0x10); // P pushed to stack with B & I flag set
    }

    SECTION("No Interrupt Vector Set") {
        // The program will crash
    }

    SECTION("BRK - Stack wrap around") {
        cpu.regs.PC = 0x1000;
        cpu.regs.flags.P = 0x20;
        cpu.regs.SP = 0x02;     // Near the bottom of the stack
        uint16_t irq_vector = 0x2000;

        ram.write(cpu.regs.PC, BRK_IMP);
        ram.write(cpu.regs.PC+1, 0xAA);
        ram.write(0xFFFE, 0x00);  // Set low byte of interrupt vector
        ram.write(0xFFFF, 0x20);  // Set high byte of interrupt vector
        cpu.execute();

        REQUIRE(cpu.regs.SP == 0xFF); // Stack pointer wraps around
        REQUIRE(cpu.regs.flags.P == 0x24); // Break flag set
        REQUIRE(cpu.regs.PC == irq_vector);
        REQUIRE(ram.read(STACK_BASE_ADDRESS + (uint8_t)(cpu.regs.SP+1)) == 0x30); // PCL pushed to stack
        REQUIRE(ram.read(STACK_BASE_ADDRESS + (uint8_t)(cpu.regs.SP+2)) == 0x02); // PCH pushed to stack
        REQUIRE(ram.read(STACK_BASE_ADDRESS + (uint8_t)(cpu.regs.SP+3)) == 0x10); // P pushed to stack with B flag set
    }
}