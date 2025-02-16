#include "ram.h"

RAM::RAM() {
    for(int i = 0; i < RAM_SIZE; ++i) {
        memory[i] = 0;
    }
    ram_cycle = 0;
}

uint8_t RAM::read(uint16_t address) {
    return memory[address];
    ram_cycle++;
}

void RAM::write(uint16_t address, uint8_t value) {
    memory[address] = value;    
    ram_cycle++;
}

bool RAM::has_page_crossed(uint16_t prev_addr, uint16_t curr_addr) {
    ram_cycle += ((prev_addr & 0xFF00) != (curr_addr & 0xFF00));
    return ram_cycle;
}

uint8_t RAM::cycle_count() {
    return ram_cycle;
}


void RAM::reset_cycle() {
    ram_cycle = 0;
}
