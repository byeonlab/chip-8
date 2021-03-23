#ifndef REGISTERS_H
#define REGISTERS_H

#include "config.h"
struct registers{
    unsigned char V[TOTAL_DATA_REGISTERS];
    unsigned short I;
    unsigned char delay_timer;
    unsigned char sound_timer;
    unsigned short PC;
    unsigned char SP;
};

#endif