#include "registers.h"


void reset_registers(Registers *regs){
    regs->af = 0;
    regs->bc = 0;
    regs->de = 0;
    regs->hl = 0;
    regs->pc = 0;
    regs->sp = 0;
}

bool get_zero_flag(Registers* regs){
    return regs->f & 0b10000000;
}
bool get_subtraction_flag(Registers* regs){
    return regs->f & 0b01000000;
}
bool get_half_carry_flag(Registers* regs){
    return regs->f & 0b00100000;
}
bool get_carry_flag(Registers* regs){
    return regs->f & 0b00010000;
}
void set_zero_flag(Registers* regs, bool value){
    regs->f = (regs->f & ~(1 << 7)) ^ (value << 7);
}
void set_subtraction_flag(Registers* regs, bool value){
    regs->f = (regs->f & ~(1 << 6)) ^ (value << 6);
}
void set_half_carry_flag(Registers* regs, bool value){
    regs->f = (regs->f & ~(1 << 5)) ^ (value << 5);
}
void set_carry_flag(Registers* regs, bool value){
    regs->f = (regs->f & ~(1 << 4)) ^ (value << 4);
}