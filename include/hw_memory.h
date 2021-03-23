#ifndef MEMROY_H
#define MEMORY_H

#include "config.h"
struct memory{
    unsigned char memory[MEMORY_SIZE];
};

void memory_set(struct memory* memory, int index, unsigned char val);
unsigned char memory_get(struct memory* memory, int index);
unsigned short memory_get_short(struct memory* memory, int index);

#endif