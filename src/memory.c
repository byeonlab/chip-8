#include "hw_memory.h"
#include <assert.h>

static void is_memory_in_bounds(int index){
    assert(index >= 0 && index < MEMORY_SIZE);
}

void memory_set(struct memory* memory, int index, unsigned char val){
    is_memory_in_bounds(index);
    memory->memory[index] = val;
}

unsigned char memory_get(struct memory* memory, int index){
    is_memory_in_bounds(index);
    return memory->memory[index];
}

unsigned short memory_get_short(struct memory* memory, int index){
    unsigned char byte1 = memory_get(memory, index);
    unsigned char byte2 = memory_get(memory, index + 1);
    return byte1 << 8 | byte2;
}