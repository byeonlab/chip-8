#include "screen.h"
#include <assert.h>
#include <memory.h>

static void screen_check_bounds(int x, int y){
    assert(x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT);
}
void screen_set(struct screen* screen, int x, int y){
    screen_check_bounds(x, y);
    screen->pixels[y][x] = true;
}

void screen_clear(struct screen* screen){
    memset(screen->pixels, 0, sizeof(screen->pixels));
}

bool screen_is_set(struct screen* screen, int x, int y){
    screen_check_bounds(x, y);
    return screen->pixels[y][x];
}

bool screen_draw_sprite(struct screen* screen, int x, int y, const char* sprite, int num){
    bool pixel_collision = false;

    for (int ly = 0; ly < num; ly++){
        char c = sprite[ly];
        for(int lx = 0; lx < 8; lx++){
            if((c & (0b10000000 >> lx)) == 0){
                continue;
            }
            if( screen->pixels[(ly + y) % HEIGHT][(lx + x) % WIDTH]){
                pixel_collision = true;
            }
            screen->pixels[(ly + y) % HEIGHT][(lx + x) % WIDTH] ^= true;
        }
    }

    return pixel_collision;
}