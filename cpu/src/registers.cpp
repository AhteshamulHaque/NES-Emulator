#include "registers.h"

Registers::Registers() {
    A = 0;
    X = 0;
    Y = 0;
    PC = 0x8000;
    SP = 0;
    P.reset();
    opcode = 0;
    operand = 0;
    addr = 0;
}

void Registers::setFlag(char identifier, bool status) {
    switch(identifier) {
        case 'C': // Carry Flag
            P[0] = status;
            break;

        case 'Z': // Zero Flag
            P[1] = status;
            break;

        case 'I': // Interrupt Disable
            P[2] = status;
            break;

        case 'D': // Decimal Mode
        P[3] = status;
            break;

        case 'B': // Break Command
            P[4] = status;
            break;

        case 'U': // Unused (always 1)
            P[5] = status;
            break;

        case 'V': // Overflow Flag
            P[6] = status;
            break;

        case 'N': // Negative Flag
            P[7] = status;
    }
}

bool Registers::getFlag(char identifier) {
    switch(identifier) {
        case 'C': // Carry Flag
            return P[0];

        case 'Z': // Zero Flag
            return P[1];

        case 'I': // Interrupt Disable
            return P[2];

        case 'D': // Decimal Mode
            return P[3];

        case 'B': // Break Command
            return P[4];

        case 'U': // Unused (always 1)
            return P[5];

        case 'V': // Overflow Flag
            return P[6];

        case 'N': // Negative Flag
            return P[7];
    }
    return false;
}

bool Registers::getNthBit(uint8_t num, uint8_t n) {
    uint8_t mask = 1u << n; // The 'u' makes it an unsigned literal
    return (num & mask) ? true : false; // Return 1 if set, 0 if not set. The 'u' ensures an unsigned result.
}
