#ifndef REGISTERS_H
#define REGISTERS_H
#include "stdint.h"
#include "stdbool.h"
    
typedef struct Registers {
    struct {
        union {
            struct {
                uint8_t f;
                uint8_t a;
            };
            uint16_t af;
        };
    };
    struct {
        union {
            struct {
                uint8_t c;
                uint8_t b;
            };
            uint16_t bc;
        };
    };
    struct {
        union {
            struct {
                uint8_t e;
                uint8_t d;
            };
            uint16_t de;
        };
    };
        
    struct {
        union {
            struct {
                uint8_t l;
                uint8_t h;
            };
            uint16_t hl;
        };
    };
    
    uint16_t pc;    // program counter
    uint16_t sp;    // stack pointer
    bool ime;       // interrupt enable
} Registers;

// Reset all registers to 0
void reset_registers(Registers *regs);

static inline bool get_zero_flag(Registers* regs){
    return regs->f & 0b10000000;
}
static inline bool get_subtraction_flag(Registers* regs){
    return regs->f & 0b01000000;
}
static inline bool get_half_carry_flag(Registers* regs){
    return regs->f & 0b00100000;
}
static inline bool get_carry_flag(Registers* regs){
    return regs->f & 0b00010000;
}
static inline void set_zero_flag(Registers* regs, bool value){
    regs->f = (regs->f & ~(1 << 7)) ^ (value << 7);
}
static inline void set_subtraction_flag(Registers* regs, bool value){
    regs->f = (regs->f & ~(1 << 6)) ^ (value << 6);
}
static inline void set_half_carry_flag(Registers* regs, bool value){
    regs->f = (regs->f & ~(1 << 5)) ^ (value << 5);
}
static inline void set_carry_flag(Registers* regs, bool value){
    regs->f = (regs->f & ~(1 << 4)) ^ (value << 4);
}


#endif
