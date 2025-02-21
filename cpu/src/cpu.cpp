#include "cpu.h"
#include "ram.h"

CPU6502::CPU6502(): ram(nullptr) {

    // initialise registers
    regs.A = 0; regs.X = 0; regs.Y = 0; regs.PC = 0x8000; regs.SP = 0xFF;

    // intialise flags
    regs.flags.C = regs.flags.Z = regs.flags.I = regs.flags.D = 0;
    regs.flags.B = regs.flags.V = regs.flags.N = 0;
    regs.flags.U = 1;

    // initialise helpful variables
    opcode = 0; addr = 0; operand = &operand_addr;

    // --- Construct Opcode Vector ---
    vector[0x00] = std::make_pair(&CPU6502::IMP, &CPU6502::BRK); vector[0x01] = std::make_pair(&CPU6502::XZI, &CPU6502::ORA);
    vector[0x02] = std::make_pair(&CPU6502::IMP, &CPU6502::KIL); vector[0x03] = std::make_pair(&CPU6502::XZI, &CPU6502::SLO);
    vector[0x04] = std::make_pair(&CPU6502::ZPG, &CPU6502::NOP); vector[0x05] = std::make_pair(&CPU6502::ZPG, &CPU6502::ORA);
    vector[0x06] = std::make_pair(&CPU6502::ZPG, &CPU6502::ASL); vector[0x07] = std::make_pair(&CPU6502::ZPG, &CPU6502::SLO);
    vector[0x08] = std::make_pair(&CPU6502::IMP, &CPU6502::PHP); vector[0x09] = std::make_pair(&CPU6502::IMM, &CPU6502::ORA);
    vector[0x0A] = std::make_pair(&CPU6502::ACC, &CPU6502::ASL); vector[0x0B] = std::make_pair(&CPU6502::IMM, &CPU6502::ANC);
    vector[0x0C] = std::make_pair(&CPU6502::ABS, &CPU6502::NOP); vector[0x0D] = std::make_pair(&CPU6502::ABS, &CPU6502::ORA);
    vector[0x0E] = std::make_pair(&CPU6502::ABS, &CPU6502::ASL); vector[0x0F] = std::make_pair(&CPU6502::ABS, &CPU6502::SLO);
    vector[0x10] = std::make_pair(&CPU6502::REL, &CPU6502::BPL); vector[0x11] = std::make_pair(&CPU6502::ZIY, &CPU6502::ORA);
    vector[0x12] = std::make_pair(&CPU6502::IMP, &CPU6502::KIL); vector[0x13] = std::make_pair(&CPU6502::ZIY, &CPU6502::SLO);
    vector[0x14] = std::make_pair(&CPU6502::XZP, &CPU6502::NOP); vector[0x15] = std::make_pair(&CPU6502::XZP, &CPU6502::ORA);
    vector[0x16] = std::make_pair(&CPU6502::XZP, &CPU6502::ASL); vector[0x17] = std::make_pair(&CPU6502::XZP, &CPU6502::SLO);
    vector[0x18] = std::make_pair(&CPU6502::IMP, &CPU6502::CLC); vector[0x19] = std::make_pair(&CPU6502::YAB, &CPU6502::ORA);
    vector[0x1A] = std::make_pair(&CPU6502::IMP, &CPU6502::NOP); vector[0x1B] = std::make_pair(&CPU6502::YAB, &CPU6502::SLO);
    vector[0x1C] = std::make_pair(&CPU6502::XAB, &CPU6502::NOP); vector[0x1D] = std::make_pair(&CPU6502::XAB, &CPU6502::ORA);
    vector[0x1E] = std::make_pair(&CPU6502::XAB, &CPU6502::ASL); vector[0x1F] = std::make_pair(&CPU6502::XAB, &CPU6502::SLO);
    vector[0x20] = std::make_pair(&CPU6502::ABS, &CPU6502::JSR); vector[0x21] = std::make_pair(&CPU6502::XZI, &CPU6502::AND);
    vector[0x22] = std::make_pair(&CPU6502::IMP, &CPU6502::KIL); vector[0x23] = std::make_pair(&CPU6502::XZI, &CPU6502::RLA);
    vector[0x24] = std::make_pair(&CPU6502::ZPG, &CPU6502::BIT); vector[0x25] = std::make_pair(&CPU6502::ZPG, &CPU6502::AND);
    vector[0x26] = std::make_pair(&CPU6502::ZPG, &CPU6502::ROL); vector[0x27] = std::make_pair(&CPU6502::ZPG, &CPU6502::RLA);
    vector[0x28] = std::make_pair(&CPU6502::IMP, &CPU6502::PLP); vector[0x29] = std::make_pair(&CPU6502::IMM, &CPU6502::AND);
    vector[0x2A] = std::make_pair(&CPU6502::ACC, &CPU6502::ROL); vector[0x2B] = std::make_pair(&CPU6502::IMM, &CPU6502::ANC);
    vector[0x2C] = std::make_pair(&CPU6502::ABS, &CPU6502::BIT); vector[0x2D] = std::make_pair(&CPU6502::ABS, &CPU6502::AND);
    vector[0x2E] = std::make_pair(&CPU6502::ABS, &CPU6502::ROL); vector[0x2F] = std::make_pair(&CPU6502::ABS, &CPU6502::RLA); 
    vector[0x30] = std::make_pair(&CPU6502::REL, &CPU6502::BMI); vector[0x31] = std::make_pair(&CPU6502::ZIY, &CPU6502::AND);
    vector[0x32] = std::make_pair(&CPU6502::IMP, &CPU6502::KIL); vector[0x33] = std::make_pair(&CPU6502::ZIY, &CPU6502::RLA);
    vector[0x34] = std::make_pair(&CPU6502::XZP, &CPU6502::NOP); vector[0x35] = std::make_pair(&CPU6502::XZP, &CPU6502::AND);
    vector[0x36] = std::make_pair(&CPU6502::XZP, &CPU6502::ROL); vector[0x37] = std::make_pair(&CPU6502::XZP, &CPU6502::RLA);
    vector[0x38] = std::make_pair(&CPU6502::IMP, &CPU6502::SEC); vector[0x39] = std::make_pair(&CPU6502::YAB, &CPU6502::AND);
    vector[0x3A] = std::make_pair(&CPU6502::IMP, &CPU6502::NOP); vector[0x3B] = std::make_pair(&CPU6502::YAB, &CPU6502::RLA);
    vector[0x3C] = std::make_pair(&CPU6502::XAB, &CPU6502::NOP); vector[0x3D] = std::make_pair(&CPU6502::XAB, &CPU6502::AND);
    vector[0x3E] = std::make_pair(&CPU6502::XAB, &CPU6502::ROL); vector[0x3F] = std::make_pair(&CPU6502::XAB, &CPU6502::RLA);
    vector[0x40] = std::make_pair(&CPU6502::IMP, &CPU6502::RTI); vector[0x41] = std::make_pair(&CPU6502::XZI, &CPU6502::EOR);
    vector[0x42] = std::make_pair(&CPU6502::IMP, &CPU6502::KIL); vector[0x43] = std::make_pair(&CPU6502::XZI, &CPU6502::SRE);
    vector[0x44] = std::make_pair(&CPU6502::ZPG, &CPU6502::NOP); vector[0x45] = std::make_pair(&CPU6502::ZPG, &CPU6502::EOR);
    vector[0x46] = std::make_pair(&CPU6502::ZPG, &CPU6502::LSR); vector[0x47] = std::make_pair(&CPU6502::ZPG, &CPU6502::SRE);
    vector[0x48] = std::make_pair(&CPU6502::IMP, &CPU6502::PHA); vector[0x49] = std::make_pair(&CPU6502::IMM, &CPU6502::EOR);
    vector[0x4A] = std::make_pair(&CPU6502::ACC, &CPU6502::LSR); vector[0x4B] = std::make_pair(&CPU6502::IMM, &CPU6502::ALR);
    vector[0x4C] = std::make_pair(&CPU6502::ABS, &CPU6502::JMP); vector[0x4D] = std::make_pair(&CPU6502::ABS, &CPU6502::EOR);
    vector[0x4E] = std::make_pair(&CPU6502::ABS, &CPU6502::LSR); vector[0x4F] = std::make_pair(&CPU6502::ABS, &CPU6502::SRE);
    vector[0x50] = std::make_pair(&CPU6502::REL, &CPU6502::BVC); vector[0x51] = std::make_pair(&CPU6502::ZIY, &CPU6502::EOR);
    vector[0x52] = std::make_pair(&CPU6502::IMP, &CPU6502::KIL); vector[0x53] = std::make_pair(&CPU6502::ZIY, &CPU6502::SRE);
    vector[0x54] = std::make_pair(&CPU6502::XZP, &CPU6502::NOP); vector[0x55] = std::make_pair(&CPU6502::XZP, &CPU6502::EOR);
    vector[0x56] = std::make_pair(&CPU6502::XZP, &CPU6502::LSR); vector[0x57] = std::make_pair(&CPU6502::XZP, &CPU6502::SRE);
    vector[0x58] = std::make_pair(&CPU6502::IMP, &CPU6502::CLI); vector[0x59] = std::make_pair(&CPU6502::YAB, &CPU6502::EOR);
    vector[0x5A] = std::make_pair(&CPU6502::IMP, &CPU6502::NOP); vector[0x5B] = std::make_pair(&CPU6502::YAB, &CPU6502::SRE);
    vector[0x5C] = std::make_pair(&CPU6502::XAB, &CPU6502::NOP); vector[0x5D] = std::make_pair(&CPU6502::XAB, &CPU6502::EOR);
    vector[0x5E] = std::make_pair(&CPU6502::XAB, &CPU6502::LSR); vector[0x5F] = std::make_pair(&CPU6502::XAB, &CPU6502::SRE);
    vector[0x60] = std::make_pair(&CPU6502::IMP, &CPU6502::RTS); vector[0x61] = std::make_pair(&CPU6502::XZI, &CPU6502::ADC);
    vector[0x62] = std::make_pair(&CPU6502::IMP, &CPU6502::KIL); vector[0x63] = std::make_pair(&CPU6502::XZI, &CPU6502::RRA);
    vector[0x64] = std::make_pair(&CPU6502::ZPG, &CPU6502::NOP); vector[0x65] = std::make_pair(&CPU6502::ZPG, &CPU6502::ADC);
    vector[0x66] = std::make_pair(&CPU6502::ZPG, &CPU6502::ROR); vector[0x67] = std::make_pair(&CPU6502::ZPG, &CPU6502::RRA);
    vector[0x68] = std::make_pair(&CPU6502::IMP, &CPU6502::PLA); vector[0x69] = std::make_pair(&CPU6502::IMM, &CPU6502::ADC);
    vector[0x6A] = std::make_pair(&CPU6502::ACC, &CPU6502::ROR); vector[0x6B] = std::make_pair(&CPU6502::IMM, &CPU6502::ARR);
    vector[0x6C] = std::make_pair(&CPU6502::ABI, &CPU6502::JMP); vector[0x6D] = std::make_pair(&CPU6502::ABS, &CPU6502::ADC);
    vector[0x6E] = std::make_pair(&CPU6502::ABS, &CPU6502::ROR); vector[0x6F] = std::make_pair(&CPU6502::ABS, &CPU6502::RRA);
    vector[0x70] = std::make_pair(&CPU6502::REL, &CPU6502::BVS); vector[0x71] = std::make_pair(&CPU6502::ZIY, &CPU6502::ADC);
    vector[0x72] = std::make_pair(&CPU6502::IMP, &CPU6502::KIL); vector[0x73] = std::make_pair(&CPU6502::ZIY, &CPU6502::RRA);
    vector[0x74] = std::make_pair(&CPU6502::XZP, &CPU6502::NOP); vector[0x75] = std::make_pair(&CPU6502::XZP, &CPU6502::ADC);
    vector[0x76] = std::make_pair(&CPU6502::XZP, &CPU6502::ROR); vector[0x77] = std::make_pair(&CPU6502::XZP, &CPU6502::RRA);
    vector[0x78] = std::make_pair(&CPU6502::IMP, &CPU6502::SEI); vector[0x79] = std::make_pair(&CPU6502::YAB, &CPU6502::ADC);
    vector[0x7A] = std::make_pair(&CPU6502::IMP, &CPU6502::NOP); vector[0x7B] = std::make_pair(&CPU6502::YAB, &CPU6502::RRA);
    vector[0x7C] = std::make_pair(&CPU6502::XAB, &CPU6502::NOP); vector[0x7D] = std::make_pair(&CPU6502::XAB, &CPU6502::ADC);
    vector[0x7E] = std::make_pair(&CPU6502::XAB, &CPU6502::ROR); vector[0x7F] = std::make_pair(&CPU6502::XAB, &CPU6502::RRA);
    vector[0x80] = std::make_pair(&CPU6502::IMM, &CPU6502::NOP); vector[0x81] = std::make_pair(&CPU6502::XZI, &CPU6502::STA);
    vector[0x82] = std::make_pair(&CPU6502::IMP, &CPU6502::KIL); vector[0x83] = std::make_pair(&CPU6502::XZI, &CPU6502::SAX);
    vector[0x84] = std::make_pair(&CPU6502::ZPG, &CPU6502::STY); vector[0x85] = std::make_pair(&CPU6502::ZPG, &CPU6502::STA);
    vector[0x86] = std::make_pair(&CPU6502::ZPG, &CPU6502::STX); vector[0x87] = std::make_pair(&CPU6502::ZPG, &CPU6502::SAX);
    vector[0x88] = std::make_pair(&CPU6502::IMP, &CPU6502::DEY); vector[0x89] = std::make_pair(&CPU6502::IMM, &CPU6502::NOP);
    vector[0x8A] = std::make_pair(&CPU6502::IMP, &CPU6502::TXA); vector[0x8B] = std::make_pair(&CPU6502::IMM, &CPU6502::ANE);
    vector[0x8C] = std::make_pair(&CPU6502::ABS, &CPU6502::STY); vector[0x8D] = std::make_pair(&CPU6502::ABS, &CPU6502::STA);
    vector[0x8E] = std::make_pair(&CPU6502::ABS, &CPU6502::STX); vector[0x8F] = std::make_pair(&CPU6502::ABS, &CPU6502::SAX);
    vector[0x90] = std::make_pair(&CPU6502::REL, &CPU6502::BCC); vector[0x91] = std::make_pair(&CPU6502::ZIY, &CPU6502::STA);
    vector[0x92] = std::make_pair(&CPU6502::IMP, &CPU6502::KIL); vector[0x93] = std::make_pair(&CPU6502::ZIY, &CPU6502::SHA);
    vector[0x94] = std::make_pair(&CPU6502::XZP, &CPU6502::STY); vector[0x95] = std::make_pair(&CPU6502::XZP, &CPU6502::STA);
    vector[0x96] = std::make_pair(&CPU6502::YZP, &CPU6502::STX); vector[0x97] = std::make_pair(&CPU6502::YZP, &CPU6502::SAX);
    vector[0x98] = std::make_pair(&CPU6502::IMP, &CPU6502::TYA); vector[0x99] = std::make_pair(&CPU6502::YAB, &CPU6502::STA);
    vector[0x9A] = std::make_pair(&CPU6502::IMP, &CPU6502::TXS); vector[0x9B] = std::make_pair(&CPU6502::YAB, &CPU6502::SHS);
    vector[0x9C] = std::make_pair(&CPU6502::XAB, &CPU6502::SHY); vector[0x9D] = std::make_pair(&CPU6502::XAB, &CPU6502::STA);
    vector[0x9E] = std::make_pair(&CPU6502::YAB, &CPU6502::SHX); vector[0x9F] = std::make_pair(&CPU6502::YAB, &CPU6502::SHA);
    vector[0xA0] = std::make_pair(&CPU6502::IMM, &CPU6502::LDY); vector[0xA1] = std::make_pair(&CPU6502::XZI, &CPU6502::LDA);
    vector[0xA2] = std::make_pair(&CPU6502::IMM, &CPU6502::LDX); vector[0xA3] = std::make_pair(&CPU6502::XZI, &CPU6502::LAX);
    vector[0xA4] = std::make_pair(&CPU6502::ZPG, &CPU6502::LDY); vector[0xA5] = std::make_pair(&CPU6502::ZPG, &CPU6502::LDA);
    vector[0xA6] = std::make_pair(&CPU6502::ZPG, &CPU6502::LDX); vector[0xA7] = std::make_pair(&CPU6502::ZPG, &CPU6502::LAX);
    vector[0xA8] = std::make_pair(&CPU6502::IMP, &CPU6502::TAY); vector[0xA9] = std::make_pair(&CPU6502::IMM, &CPU6502::LDA);
    vector[0xAA] = std::make_pair(&CPU6502::IMP, &CPU6502::TAX); vector[0xAB] = std::make_pair(&CPU6502::IMM, &CPU6502::LXA);
    vector[0xAC] = std::make_pair(&CPU6502::ABS, &CPU6502::LDY); vector[0xAD] = std::make_pair(&CPU6502::ABS, &CPU6502::LDA);
    vector[0xAE] = std::make_pair(&CPU6502::ABS, &CPU6502::LDX); vector[0xAF] = std::make_pair(&CPU6502::ABS, &CPU6502::LAX);
    vector[0xB0] = std::make_pair(&CPU6502::REL, &CPU6502::BCS); vector[0xB1] = std::make_pair(&CPU6502::ZIY, &CPU6502::LDA);
    vector[0xB2] = std::make_pair(&CPU6502::IMP, &CPU6502::KIL); vector[0xB3] = std::make_pair(&CPU6502::ZIY, &CPU6502::LAX);
    vector[0xB4] = std::make_pair(&CPU6502::XZP, &CPU6502::LDY); vector[0xB5] = std::make_pair(&CPU6502::XZP, &CPU6502::LDA);
    vector[0xB6] = std::make_pair(&CPU6502::YZP, &CPU6502::LDX); vector[0xB7] = std::make_pair(&CPU6502::YZP, &CPU6502::LAX);
    vector[0xB8] = std::make_pair(&CPU6502::IMP, &CPU6502::CLV); vector[0xB9] = std::make_pair(&CPU6502::YAB, &CPU6502::LDA);
    vector[0xBA] = std::make_pair(&CPU6502::IMP, &CPU6502::TSX); vector[0xBB] = std::make_pair(&CPU6502::YAB, &CPU6502::LAS);
    vector[0xBC] = std::make_pair(&CPU6502::XAB, &CPU6502::LDY); vector[0xBD] = std::make_pair(&CPU6502::XAB, &CPU6502::LDA);
    vector[0xBE] = std::make_pair(&CPU6502::YAB, &CPU6502::LDX); vector[0xBF] = std::make_pair(&CPU6502::YAB, &CPU6502::LAX);
    vector[0xC0] = std::make_pair(&CPU6502::IMM, &CPU6502::CPY); vector[0xC1] = std::make_pair(&CPU6502::XZI, &CPU6502::CMP);
    vector[0xC2] = std::make_pair(&CPU6502::IMM, &CPU6502::NOP); vector[0xC3] = std::make_pair(&CPU6502::XZI, &CPU6502::DCP);
    vector[0xC4] = std::make_pair(&CPU6502::ZPG, &CPU6502::CPY); vector[0xC5] = std::make_pair(&CPU6502::ZPG, &CPU6502::CMP);
    vector[0xC6] = std::make_pair(&CPU6502::ZPG, &CPU6502::DEC); vector[0xC7] = std::make_pair(&CPU6502::ZPG, &CPU6502::DCP);
    vector[0xC8] = std::make_pair(&CPU6502::IMP, &CPU6502::INY); vector[0xC9] = std::make_pair(&CPU6502::IMM, &CPU6502::CMP);
    vector[0xCA] = std::make_pair(&CPU6502::IMP, &CPU6502::DEX); vector[0xCB] = std::make_pair(&CPU6502::IMM, &CPU6502::AXS);
    vector[0xCC] = std::make_pair(&CPU6502::ABS, &CPU6502::CPY); vector[0xCD] = std::make_pair(&CPU6502::ABS, &CPU6502::CMP);
    vector[0xCE] = std::make_pair(&CPU6502::ABS, &CPU6502::DEC); vector[0xCF] = std::make_pair(&CPU6502::ABS, &CPU6502::DCP);
    vector[0xD0] = std::make_pair(&CPU6502::REL, &CPU6502::BNE); vector[0xD1] = std::make_pair(&CPU6502::ZIY, &CPU6502::CMP);
    vector[0xD2] = std::make_pair(&CPU6502::IMP, &CPU6502::KIL); vector[0xD3] = std::make_pair(&CPU6502::ZIY, &CPU6502::DCP);
    vector[0xD4] = std::make_pair(&CPU6502::XZP, &CPU6502::NOP); vector[0xD5] = std::make_pair(&CPU6502::XZP, &CPU6502::CMP);
    vector[0xD6] = std::make_pair(&CPU6502::XZP, &CPU6502::DEC); vector[0xD7] = std::make_pair(&CPU6502::XZP, &CPU6502::DCP);
    vector[0xD8] = std::make_pair(&CPU6502::IMP, &CPU6502::CLD); vector[0xD9] = std::make_pair(&CPU6502::YAB, &CPU6502::CMP);
    vector[0xDA] = std::make_pair(&CPU6502::IMP, &CPU6502::NOP); vector[0xDB] = std::make_pair(&CPU6502::YAB, &CPU6502::DCP);
    vector[0xDC] = std::make_pair(&CPU6502::XAB, &CPU6502::NOP); vector[0xDD] = std::make_pair(&CPU6502::XAB, &CPU6502::CMP);
    vector[0xDE] = std::make_pair(&CPU6502::XAB, &CPU6502::DEC); vector[0xDF] = std::make_pair(&CPU6502::XAB, &CPU6502::DCP);
    vector[0xE0] = std::make_pair(&CPU6502::IMM, &CPU6502::CPX); vector[0xE1] = std::make_pair(&CPU6502::XZI, &CPU6502::SBC);
    vector[0xE2] = std::make_pair(&CPU6502::IMM, &CPU6502::NOP); vector[0xE3] = std::make_pair(&CPU6502::XZI, &CPU6502::ISC);
    vector[0xE4] = std::make_pair(&CPU6502::ZPG, &CPU6502::CPX); vector[0xE5] = std::make_pair(&CPU6502::ZPG, &CPU6502::SBC);
    vector[0xE6] = std::make_pair(&CPU6502::ZPG, &CPU6502::INC); vector[0xE7] = std::make_pair(&CPU6502::ZPG, &CPU6502::ISC);
    vector[0xE8] = std::make_pair(&CPU6502::IMP, &CPU6502::INX); vector[0xE9] = std::make_pair(&CPU6502::IMM, &CPU6502::SBC);
    vector[0xEA] = std::make_pair(&CPU6502::IMP, &CPU6502::NOP); vector[0xEB] = std::make_pair(&CPU6502::IMM, &CPU6502::SBC);
    vector[0xEC] = std::make_pair(&CPU6502::ABS, &CPU6502::CPX); vector[0xED] = std::make_pair(&CPU6502::ABS, &CPU6502::SBC);
    vector[0xEE] = std::make_pair(&CPU6502::ABS, &CPU6502::INC); vector[0xEF] = std::make_pair(&CPU6502::ABS, &CPU6502::ISC);
    vector[0xF0] = std::make_pair(&CPU6502::REL, &CPU6502::BEQ); vector[0xF1] = std::make_pair(&CPU6502::ZIY, &CPU6502::SBC);
    vector[0xF2] = std::make_pair(&CPU6502::IMP, &CPU6502::KIL); vector[0xF3] = std::make_pair(&CPU6502::ZIY, &CPU6502::ISC);
    vector[0xF4] = std::make_pair(&CPU6502::XZP, &CPU6502::NOP); vector[0xF5] = std::make_pair(&CPU6502::XZP, &CPU6502::SBC);
    vector[0xF6] = std::make_pair(&CPU6502::XZP, &CPU6502::INC); vector[0xF7] = std::make_pair(&CPU6502::XZP, &CPU6502::ISC);
    vector[0xF8] = std::make_pair(&CPU6502::IMP, &CPU6502::SED); vector[0xF9] = std::make_pair(&CPU6502::YAB, &CPU6502::SBC);
    vector[0xFA] = std::make_pair(&CPU6502::IMP, &CPU6502::NOP); vector[0xFB] = std::make_pair(&CPU6502::YAB, &CPU6502::ISC);
    vector[0xFC] = std::make_pair(&CPU6502::XAB, &CPU6502::NOP); vector[0xFD] = std::make_pair(&CPU6502::XAB, &CPU6502::SBC);
    vector[0xFE] = std::make_pair(&CPU6502::XAB, &CPU6502::INC); vector[0xFF] = std::make_pair(&CPU6502::XAB, &CPU6502::ISC);
}

void CPU6502::fetch() {
    opcode = ram->read(regs.PC); 
    regs.PC++;
}

void CPU6502::connect(RAM &ram) {
    this->ram = &ram;
}

bool CPU6502::getNthBit(uint8_t num, uint8_t n) {
    uint8_t mask = 1u << n; // The 'u' makes it an unsigned literal
    return (num & mask);
}

void CPU6502::stack_push(uint8_t value) {
    ram->write(STACK_BASE_ADDRESS + regs.SP, value);
    --regs.SP;
}

uint8_t CPU6502::stack_pop() {
    ++regs.SP;
    return ram->read(STACK_BASE_ADDRESS + regs.SP);
}

/*******************************************************************
 ******************* ADDRESSING MODE EMULATION *********************
 *******************************************************************
 *
 * These emulation takes care of fetching and decoding the information
 * to be used by the Instruction set.
 */

void CPU6502::IMP() {}

void CPU6502::ACC() {
    operand_addr = &regs.A;
}

void CPU6502::IMM() {
    // ADC #$nn
    addr = regs.PC;
    operand_addr = ram->get_address(addr);
    fetch(); // to advance to PC
}

void CPU6502::ZPG() {
    // ADC $nn
    // M = memory[$nn]
    fetch();
    addr = opcode;
    operand_addr = ram->get_address(addr);
}

void CPU6502::XZP() {
    // ADC $nn, X
    // M = memory[$nn+X]
    fetch();
    addr = (uint8_t)(opcode + regs.X);
    operand_addr = ram->get_address(addr);
}

void CPU6502::YZP() {
    // LDX $nn, Y
    // M = memory[$nn+Y]
    fetch();
    addr = (uint8_t)(opcode + regs.Y);
    operand_addr = ram->get_address(addr);
}

void CPU6502::ABS() {
    // ADC $nnnn
    // M = memory[$nnnn]
    fetch();
    addr = opcode;
    fetch();
    addr |= (opcode << 8);
    operand_addr = ram->get_address(addr);
}

void CPU6502::XAB() {
    // ADC $nnnn, X
    // M = memory[$nnnn+X]
    fetch();
    addr = opcode;
    fetch();
    addr |= (opcode << 8);
    ram->has_page_crossed(addr, addr+regs.X);
    addr += regs.X;
    operand_addr = ram->get_address(addr);
}

void CPU6502::YAB() {
    // ADC $nnnn, Y
    // M = memory[$nnnn+Y]
    fetch();
    addr = opcode;
    fetch();
    addr |= (opcode << 8);
    ram->has_page_crossed(addr, addr+regs.X);
    addr += regs.Y;
    operand_addr = ram->get_address(addr);
}

void CPU6502::ABI() {
    // JMP ($nnnn)
    // M     = 0 x memory[$nnnn+1] memory[$nnnn]
    // Eg: M = 0 x       45            76
    //     M = 0x4576
    fetch();
    addr = opcode;
    fetch();
    addr |= (opcode << 8);

    uint16_t tmp_addr;

    // fetch the indirect address
    tmp_addr = ram->read(addr);
    tmp_addr |= (ram->read((addr+1)) << 8);

    addr = tmp_addr;
    operand_addr = ram->get_address(tmp_addr);
}

void CPU6502::XZI() {
    // ADC ($nn), X
    // addr = $nn+X
    // M     = 0 x memory[$addr] memory[$addr+1]
    // Eg: M = 0 x      45             76
    //     M = 0x4576
    fetch();
    addr = (uint8_t)(opcode + regs.X);

    uint16_t tmp_addr;

    // fetch the indirect address
    tmp_addr = ram->read(addr);
    tmp_addr |= (ram->read((uint8_t)(addr+1)) << 8); // zero page access only

    addr = tmp_addr;
    operand_addr = ram->get_address(tmp_addr);
}

void CPU6502::ZIY() {
    // ADC $(nn), Y
    // addr = 0 x memory[$nn+1] memory[$nn]
    // M = memory[$addr+Y]
    fetch();
    addr = ram->read(opcode);
    addr |= (ram->read((uint8_t)(opcode+1)) << 8); // zero page access only
    ram->has_page_crossed(addr, addr+regs.Y);
    addr += regs.Y;
    operand_addr = ram->get_address(addr);
}

void CPU6502::REL() {
    // This instruction only used with Branch instructions
    // BCC $abcd
    // PC += $ab ($ab is signed 8 bit -> to move forward/backward)
    fetch();
    addr = opcode;
    operand_addr = nullptr;
    fetch();    // to go past the third byte
    // implement the rest of the logic in branch inst i.e
    // converting it into signed 8 bit and using it then.
}

/************************************************************************
 ****************** INSTRUCTION SET EMULATION ***************************
 ************************************************************************/
void CPU6502::LDA() {
    // A = M
    regs.A = **operand;

    regs.flags.Z = (regs.A == 0);
    regs.flags.N = (getNthBit(regs.A, 7));
    cpu_cycle++;
}

void CPU6502::ADC() {
    // A = A + M + C

    uint16_t result = regs.A + (**operand) + regs.flags.C;

    /* 0x00 - 0x7F represents 0 to 127 in signed integer
     * 0x80 - 0xFF represents -128 to -1 in signed integer
     * Conditions to set V flag:
     *      1. If two positive numbers overflow
     *      2. If two negative numbers overflow
     */
    regs.flags.V = ((regs.A <= 0x7F && **operand <= 0x7F && result > 0x7F) ||
                    (regs.A >= 0x80 && **operand >= 0x80 && result >= 0xFF));
    regs.flags.C = (result > 0xFF);
    regs.A = result;
    regs.flags.Z = (regs.A == 0);
    regs.flags.N = getNthBit(regs.A, 7);

    cpu_cycle++;
}

void CPU6502::AND() {
    // A = A & M
    regs.A &= **operand;

    regs.flags.Z = (regs.A == 0);
    regs.flags.N = getNthBit(regs.A, 7);
}

void CPU6502::ASL() {
    // operand will point to memory or accumulator depending on
    // addressing mode
    // [M] = operand << 1 (writes back to memory)
    // A = A << 1

    // since `**operand` is a 8 bit value it will lose its 7th bit
    // after shifting, hence saving it in C flag before the operation
    regs.flags.C = getNthBit(**operand, 7);

    **operand <<= 1;

    regs.flags.N = getNthBit(**operand, 7);
    regs.flags.Z = (**operand == 0);
}

void CPU6502::BCC() {
    // PC = PC + 2(added in addressing mode) + M
    if(!regs.flags.C) {
        regs.PC += (int8_t)addr;
    }
}

void CPU6502::BCS() {
    // PC = PC + 2(added in addressing mode) + M
    if(regs.flags.C) {
        regs.PC += (int8_t)addr;
    }
}

void CPU6502::BEQ() {
    // PC = PC + 2(added in addressing mode) + M
    if(regs.flags.Z) {
        regs.PC += (int8_t)addr;
    }
}

void CPU6502::BIT() {
    uint8_t result = regs.A & **operand;
    regs.flags.Z = (result == 0);
    regs.flags.V = (getNthBit(**operand, 6));
    regs.flags.N = (getNthBit(**operand, 7));
}

void CPU6502::BMI() {
    // PC = PC + 2(added in addressing mode) + M
    if(regs.flags.N) {
        regs.PC += (int8_t)addr;
    }
}

void CPU6502::BNE() {
    // PC = PC + 2(added in addressing mode) + M
    if(!regs.flags.Z) {
        regs.PC += (int8_t)addr;
    }
}

void CPU6502::BPL() {
    // PC = PC + 2(added in addressing mode) + M
    if(!regs.flags.N) {
        regs.PC += (int8_t)addr;
    }
}

void CPU6502::BRK() {
    /* BRK is an implied addressing mode instruction but the second
     * byte that is immediate is skipped and PC + 2 is saved on the
     * stack.
     * push PC + 2 to stack
     * push NV11DIZC flags to stack 
     * PC = ($FFFE)
     */
    fetch(); // advance the PC, ignoring the opcode
    uint8_t PC_high_byte = (regs.PC >> 8) & 0xFF; // Get the upper 8 bits
    uint8_t PC_low_byte = regs.PC & 0xFF;        // Get the lower 8 bits

    stack_push(PC_high_byte);
    stack_push(PC_low_byte);

    bool saved_B_flag = regs.flags.B;
    regs.flags.B = 1;
    stack_push(regs.flags.P);

    regs.flags.B = saved_B_flag;
    regs.flags.I = 1;
    regs.PC = ram->read(0xFFFE);
    regs.PC |= (ram->read(0xFFFF) << 8);
}

void CPU6502::BVC() {
    // PC = PC + 2(added in addressing mode) + M
    if(!regs.flags.V) {
        regs.PC += (int8_t)addr;
    }
}

void CPU6502::BVS() {
    // PC = PC + 2(added in addressing mode) + M
    if(regs.flags.V) {
        regs.PC += (int8_t)addr;
    }
}

void CPU6502::CLC() {
    regs.flags.C = 0;
}

void CPU6502::CLD() {
    regs.flags.D = 0;
}

void CPU6502::CLI() {
    regs.flags.I = 0;
}

void CPU6502::CLV() {
    regs.flags.V = 0;
}

void CPU6502::CMP() {
    regs.flags.C = (regs.A >= **operand);
    regs.flags.Z = (regs.A == **operand);
    regs.flags.N = getNthBit(regs.A - **operand, 7);
}

void CPU6502::CPX() {
    regs.flags.C = (regs.X >= **operand);
    regs.flags.Z = (regs.X == **operand);
    regs.flags.N = getNthBit(regs.X - **operand, 7);
}

void CPU6502::CPY() {
    regs.flags.C = (regs.Y >= **operand);
    regs.flags.Z = (regs.Y == **operand);
    regs.flags.N = getNthBit(regs.Y - **operand, 7);
}

void CPU6502::DEC() {
    **operand -= 1;
    regs.flags.Z = (**operand == 0);
    regs.flags.N = getNthBit(**operand, 7);
}

void CPU6502::DEX() {
    regs.X--;
    regs.flags.Z = (regs.X == 0);
    regs.flags.N = getNthBit(regs.X, 7);
}

void CPU6502::DEY() {
    regs.Y--;
    regs.flags.Z = (regs.Y == 0);
    regs.flags.N = getNthBit(regs.Y, 7);
}

void CPU6502::EOR() {
    regs.A = regs.A ^ **operand;
    regs.flags.Z = (regs.A == 0);
    regs.flags.N = getNthBit(regs.A, 7);
}

void CPU6502::INC() {
    **operand += 1;
    regs.flags.Z = (**operand == 0);
    regs.flags.N = getNthBit(**operand, 7);
}

void CPU6502::INX() {
    regs.X++;
    regs.flags.Z = (regs.X == 0);
    regs.flags.N = getNthBit(regs.X, 7);
}

void CPU6502::INY() {
    regs.Y++;
    regs.flags.Z = (regs.Y == 0);
    regs.flags.N = getNthBit(regs.Y, 7);
}

void CPU6502::JMP() {
    regs.PC = addr;
}

void CPU6502::JSR() {
    // push PC + 2 to stack
    // PC = memory
    uint8_t PC_high_byte = (regs.PC >> 8) & 0xFF; // Get the upper 8 bits
    uint8_t PC_low_byte = regs.PC & 0xFF;        // Get the lower 8 bits

    stack_push(PC_high_byte);
    stack_push(PC_low_byte);

    regs.PC = addr;
}

void CPU6502::LDX() {
    regs.X = **operand;
    regs.flags.Z = (regs.X == 0);
    regs.flags.N = getNthBit(regs.X, 7);
}

void CPU6502::LDY() {
    regs.Y = **operand;
    regs.flags.Z = (regs.Y == 0);
    regs.flags.N = getNthBit(regs.Y, 7);
}

void CPU6502::LSR() {
    **operand >>= 1;
    regs.flags.C = getNthBit(**operand, 0);
    regs.flags.Z = (**operand == 0);
    regs.flags.N = getNthBit(**operand, 7);
}

void CPU6502::NOP() {
    // TODO
}

void CPU6502::ORA() {
    // A = A | memory
    regs.A |= **operand;
    regs.flags.Z = (regs.A == 0);
    regs.flags.N = getNthBit(regs.A, 7);
}

void CPU6502::PHA() {
    stack_push(regs.A);
}

void CPU6502::PHP() {
    // ($0100 + SP) = NV11DIZC
    // SP = SP - 1
    bool saved_B_flag = regs.flags.B;
    regs.flags.B = 1;
    stack_push(regs.flags.P);

    regs.flags.B = saved_B_flag;
}

void CPU6502::PLA() {
    // SP = SP + 1
    // A = ($0100 + SP)
    regs.A = stack_pop();

    regs.flags.Z = (regs.A == 0);
    regs.flags.N = (getNthBit(regs.A, 7));
}

void CPU6502::PLP() {
    // SP = SP + 1
    // NVxxDIZC = ($0100 + SP)
    regs.flags.P = stack_pop();
    
    // Note: Note that the effect of changing I is delayed
    // one instruction because the flag is changed after IRQ
    // is polled, delaying the effect until IRQ is polled in
    //the next instruction like with CLI and SEI.
    // TODO - maybe delay setting of I flag till next instruction
}

void CPU6502::ROL() {
    // C <- [76543210] <- C
    regs.flags.C = getNthBit(**operand, 7);
    **operand <<= 1;
    **operand = (regs.flags.C) ? (**operand | 0x01) : (**operand & 0xFE);
}

void CPU6502::ROR() {
    // C -> [76543210] -> C
    regs.flags.C = getNthBit(**operand, 0);
    **operand >>= 1;
    **operand = (regs.flags.C) ? (**operand | 0x80) : (**operand & 0x7F);
}

void CPU6502::RTI() {
    // pull NVxxDIZC flags from stack
    // pull PC from stack
    regs.flags.P = stack_pop();
    
    regs.PC = stack_pop();
    regs.PC |= (stack_pop() << 8);
}

void CPU6502::RTS() {
    // pull PC from stack
    // PC = PC + 1
    regs.PC = stack_pop();
    regs.PC |= (stack_pop() << 8); 

    regs.PC++;
}

void CPU6502::SBC() {
    // A = A + ~memory + C
    uint16_t result = regs.A - **operand + regs.flags.C;
    regs.A = (uint8_t)result;
    regs.flags.C = !(result < 0x00);
    regs.flags.Z = (regs.A == 0);
    regs.flags.N = (getNthBit(regs.A, 7)); 
    regs.flags.V = ((regs.A <= 0x7F && **operand <= 0x7F && result > 0x7F) ||
                    (regs.A >= 0x80 && **operand >= 0x80 && result >= 0xFF));
}

void CPU6502::SEC() {
    regs.flags.C = 1;
}

void CPU6502::SED() {
    regs.flags.D = 1;
}

void CPU6502::SEI() {
    regs.flags.I = 1;
}

void CPU6502::STA() {
    **operand = regs.A;
}

void CPU6502::STX() {
    **operand = regs.X;
}

void CPU6502::STY() {
    **operand = regs.Y;
}

void CPU6502::TAX() {
    regs.X = regs.A;

    regs.flags.Z = (regs.X == 0);
    regs.flags.N = getNthBit(regs.X, 7);
}

void CPU6502::TAY() {
    regs.Y = regs.A;

    regs.flags.Z = (regs.Y == 0);
    regs.flags.N = getNthBit(regs.Y, 7);
}

void CPU6502::TSX() {
    regs.X = regs.SP;

    regs.flags.Z = (regs.X == 0);
    regs.flags.N = getNthBit(regs.X, 7);
}

void CPU6502::TXA() {
    regs.A = regs.X;

    regs.flags.Z = (regs.A == 0);
    regs.flags.N = getNthBit(regs.A, 7);
}

void CPU6502::TXS() {
    regs.SP = regs.X;
}

void CPU6502::TYA() {
    regs.A = regs.Y;

    regs.flags.Z = (regs.A == 0);
    regs.flags.N = getNthBit(regs.A, 7);
}


/********************************************************************
 ******************* UNDOCUMENTED INSTRUCTIONS **********************
 ********************************************************************/
void CPU6502::KIL() {}
void CPU6502::SLO() {}
void CPU6502::ANC() {}
void CPU6502::RLA() {}
void CPU6502::SAX() {}
void CPU6502::SRE() {}
void CPU6502::ALR() {}
void CPU6502::RRA() {}
void CPU6502::ARR() {}
void CPU6502::ANE() {}
void CPU6502::LAX() {}
void CPU6502::LXA() {}
void CPU6502::LAS() {}
void CPU6502::DCP() {}
void CPU6502::SHA() {}
void CPU6502::SHX() {}
void CPU6502::SHS() {}
void CPU6502::SHY() {}
void CPU6502::ISC() {}
void CPU6502::AXS() {}


uint8_t CPU6502::execute() {

    // fetch the instruction
    fetch();

    // save the opcode because it changes during decode phase
    uint8_t idx = opcode;
    
    // decode the instruction
    (this->*vector[idx].first)();

    // execute the instruction
    (this->*vector[idx].second)();

    return ram->cycle_count() + cpu_cycle;
}
