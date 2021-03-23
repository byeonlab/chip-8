#ifndef SCREEN_H
#define SCREEN_H

#include <stdbool.h>
#include "config.h"

struct screen
{
    bool pixels[HEIGHT][WIDTH];
};

void screen_clear(struct screen* screen);
void screen_set(struct screen* screen, int x, int y);
bool screen_is_set(struct screen* screen, int x, int y);
bool screen_draw_sprite(struct screen* screen, int x, int y, const char* sprite, int num);

#endif