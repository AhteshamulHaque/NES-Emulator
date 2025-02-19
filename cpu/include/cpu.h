#ifndef CPU6502_H
#define CPU6502_H

#include <iostream>
#include <cstdint>
#include "registers.h"
#include "ram.h"

class CPU6502 {
    private:
        uint8_t opcode; // current fetched instruction from memory
        /*
         * `operand_addr` is the address at which operand is found.
         * It helps in pointing to either RAM memory address or
         * ACCUMULATOR address depending on the addressing mode
         */
        uint8_t *operand_addr;
        uint8_t **operand;  // points to `operand_addr`. (optional)
        // uint8_t operand;
        uint16_t addr;   // for doing calculations for the CPU
        uint8_t cpu_cycle; // cycle count to execute the cpu instruction

    public:
        struct StatusFlags {
            bool C; // Carry Flag
            bool Z; // Zero Flag
            bool I; // Interrupt Disable
            bool D; // Decimal Mode
            bool B; // Break Command
            bool U; // Unused (always 1)
            bool V; // Overflow Flag
            bool N; // Negative Flag
        };

        struct Registers {
            uint8_t A;  // Accumulator
            uint8_t X;  // X Register
            uint8_t Y;  // Y Register
            uint8_t SP; // Stack Pointer
            uint16_t PC; // Program Counter
            StatusFlags flags;  // P Register
        };

        // components of CPU
        Registers regs;
        RAM *ram;

        bool getNthBit(uint8_t, uint8_t);
        void fetch();

        std::pair<void (CPU6502::*)(),void (CPU6502::*)()> vector[256];

        CPU6502();

        /* Bus connection to different components
         * TODO: connect to PPU, APU, CONTROLLERS, CARTRIDGE
         */
        void connect(RAM &ram);
        
        /* Instructions of 6502 CPU
         * XXX() is used for debugging the cpu
         */
        
        void ADC(); void AND(); void ASL(); void BCC(); void BCS();
        void BEQ(); void BIT(); void BMI(); void BNE(); void BPL();
        void BRK(); void BVC(); void BVS(); void CLC(); void CLD();
        void CLI(); void CLV(); void CMP(); void CPX();	void CPY();
        void DEC(); void DEX(); void DEY(); void EOR(); void INC();
        void INX(); void INY(); void JMP(); void JSR(); void LDA();
        void LDX(); void LDY(); void LSR(); void NOP(); void ORA();
        void PHA(); void PHP(); void PLA(); void PLP(); void ROL();
        void ROR(); void RTI(); void RTS(); void SBC(); void SEC();
        void SED(); void SEI(); void STA(); void STX(); void STY();
        void TAX(); void TAY(); void TSX(); void TXA(); void TXS();
        void TYA();

        // Illegal Undocumented Instructions
        void KIL(); void SLO(); void ANC(); void RLA(); void SAX();
        void SRE(); void ALR(); void RRA(); void ARR(); void ANE();
        void LAX(); void LXA(); void LAS(); void DCP(); void SHA();
        void SHX(); void SHS(); void SHY(); void ISC(); void AXS();
            

        /* Addressing modes:
         * IMP - Implied
         * IMM - Immediate
         * ZPG - Zero Page
         * XZP - X-Indexed Zero Page
         * YZP - Y-Indexed Zero Page
         * ABS - Absolute
         * XAB - X-Indexed Absolute
         * YAB - Y-Indexed Absolute
         * ABI - Absolute Indirect
         * XZI - X-Indexed Zero Page Indirect
         * ZIY - Zero Page Indirect Y-Indexed
         * REL - Relative
         * ACC - Accumulator
         *
         * It fetchs & decodes instruction from memory and stores the
         * fetched/decoded value in register M to be used by the actual
         * instruction such as LDA, ADC, etc.
         *
         * @return - only cycle count to fetch the contents
         */
        void IMP(); void IMM(); void ZPG(); void XZP(); void YZP();
        void ABS(); void XAB(); void YAB(); void ABI(); void XZI(); 
        void ZIY(); void REL(); void ACC();

        // Executes the next instruction in the ram 
        uint8_t execute();
};

#endif // !CPU6502_H