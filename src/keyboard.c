#include "hw_keyboard.h"
#include <assert.h>
static void keyboard_ensure_in_bounds(int key){
    assert(key >= 0 && key < TOTAL_KEYS);
}

void keyboard_set_map(struct keyboard* keyboard, const char* map){
    keyboard->keyboard_map = map;
}

int keyboard_map(const char* map, char key){
    for (int i = 0; i < TOTAL_KEYS; i++){
        if (map[i] == key) {
            return i;
        }
    }
    return -1;
}

void keyboard_down(struct keyboard* keyboard, int key){
    keyboard->keyboard[key] = true;
}

void keyboard_up(struct keyboard* keyboard, int key){
    keyboard->keyboard[key] = false;
}

bool keyboard_is_down(struct keyboard* keyboard, int key){
    return keyboard->keyboard[key];
}