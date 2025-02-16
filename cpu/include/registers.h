#ifndef REGISTERS_H
#define REGISTERS_H

#include <bitset>

class Registers {

    public:
        uint8_t A;  // Accumulator
        uint8_t X;  // X Register
        uint8_t Y;  // Y Register
        uint8_t SP; // Stack Pointer

        /* `opcode` and `operand` are two special variables that holds intermediate
         * values `opcode` is intended to store the CURRENT fetched instruction and
         * `operand` hold the intermediate value as well as final values.
         * `operand` is actually used by the INSTRUCTION SET EMULATION functions &
         */
        uint8_t opcode; // Current instruction to execute
        uint8_t operand; // value used by an instruction
        uint16_t addr;  // ABIirect/direct address
        uint16_t PC; // Program Counter

        std::bitset<8> P; // Status Register (Flags)

        Registers();
        void setFlag(char identifier, bool status);
        bool getFlag(char identifier);
        bool getNthBit(uint8_t num, uint8_t n);
};

#endif // !REGISTERS_H
