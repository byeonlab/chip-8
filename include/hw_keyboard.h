#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdbool.h>
#include "config.h"
struct keyboard{
    bool keyboard[TOTAL_KEYS];
    const char* keyboard_map;
};

void keyboard_set_map(struct keyboard* keyboard, const char* map);
int keyboard_map(const char* map, char key);
void keyboard_down(struct keyboard* keyboard, int key);
void keyboard_up(struct keyboard* keyboard, int key);
bool keyboard_is_down(struct keyboard* keyboard, int key);

#endif
