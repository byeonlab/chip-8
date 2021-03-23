#ifndef STACK_H
#define STACK_H

#include "config.h"

struct chip8;
struct stack{
    unsigned short stack[TOTAL_STACK_DEPTH];
};

void stack_push(struct chip8* chip8, unsigned short val);
unsigned short stack_pop(struct chip8* chip8);

#endif