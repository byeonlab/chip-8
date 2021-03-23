#ifndef CHIP8_H
#define CHIP8_H

#include<stddef.h>
#include "config.h"
#include "hw_memory.h"
#include "registers.h"
#include "stack.h"
#include "hw_keyboard.h"
#include "screen.h"
struct chip8{
    struct memory memory;
    struct stack stack;
    struct registers registers;
    struct keyboard keyboard;
    struct screen screen;
};

void init(struct chip8* chip8);
void load(struct chip8* chip8, const char* buf, size_t size);
void exec(struct chip8* chip8, unsigned short opcode);

#endif