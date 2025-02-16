#ifndef RAM_H
#define RAM_H

#define RAM_SIZE 65536

#include <cstdint>

class RAM {
    uint8_t memory[RAM_SIZE];

    /* It is incremented on every ram access.
       NOTE: It needs to be reset after every instruction
       execution
     */
    uint8_t ram_cycle;

    public:
        RAM();
        uint8_t read(uint16_t);
        void write(uint16_t, uint8_t);
        uint8_t cycle_count();
        // used by cpu addressing modes to check for page boundary cross
        bool has_page_crossed(uint16_t, uint16_t);
        void reset_cycle(); // reset the cycle after a single execution
};

#endif // !MEMORY_H
