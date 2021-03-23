#include <stdio.h>
#include "SDL2/SDL.h"
#include "chip8.h"
#include "hw_keyboard.h"

//#include <unistd.h>
#include <windows.h>

const char total_keyboard_map[TOTAL_KEYS] = {
    SDLK_0, SDLK_1, SDLK_2, SDLK_3,
    SDLK_4, SDLK_5, SDLK_6, SDLK_7,
    SDLK_8, SDLK_9, SDLK_a, SDLK_b,
    SDLK_c, SDLK_d, SDLK_e, SDLK_f
};

int main(int argc, char** argv) {

    if (argc < 2){
        printf("You must provide a file to load\n");
        return -1;
    }

    const char* filename = argv[1]; 
    printf("Loading %s...\n", filename);
    FILE* f = fopen(filename, "rb");
    if(!f){
        printf("Failed to open the file\n");
        return -1;
    }
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    char buf[size];
    int res = fread(buf, size, 1, f);
    if(res != 1){
        printf("Failed to read from file\n");
        return -1;
    }

    struct chip8 chip8;
    init(&chip8);
    load(&chip8, buf, size);
    keyboard_set_map(&chip8.keyboard, total_keyboard_map);

    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window* window = SDL_CreateWindow(
        EMULATOR_WINDOW_TITLE,
        SDL_WINDOWPOS_UNDEFINED, 
        SDL_WINDOWPOS_UNDEFINED,
        WIDTH * SCREEN_SCALER, 
        HEIGHT * SCREEN_SCALER, SDL_WINDOW_SHOWN
    );

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_TEXTUREACCESS_TARGET);
    while(1) {
        SDL_Event event;
        while(SDL_PollEvent(&event)){
            switch (event.type)
            {
            case SDL_QUIT:
                goto out;
            break;
            
            case SDL_KEYDOWN:
            {
                char key = event.key.keysym.sym;
                int vkey = keyboard_map(total_keyboard_map, key);
                if (vkey != -1){
                    keyboard_down(&chip8.keyboard, vkey);
                }
            }
            break;

            case SDL_KEYUP:{
                char key = event.key.keysym.sym;
                int vkey = keyboard_map(total_keyboard_map, key);
                if (vkey != -1){
                    keyboard_up(&chip8.keyboard, vkey);
                }
            }
            break;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);

        for (int x = 0; x < WIDTH; x++){
            for (int y = 0; y <HEIGHT; y++){
                if (screen_is_set(&chip8.screen, x, y)){
                    SDL_Rect r;
                    r.x = x * SCREEN_SCALER;
                    r.y = y * SCREEN_SCALER;
                    r.w = SCREEN_SCALER;
                    r.h = SCREEN_SCALER;
                    //SDL_RenderDrawRect(renderer, &r);
                    SDL_RenderFillRect(renderer, &r);
                }
            }
        }
        SDL_RenderPresent(renderer);

        if(chip8.registers.delay_timer > 0){
            Sleep(100);
            chip8.registers.delay_timer -= 1;
        }
        if(chip8.registers.sound_timer > 0){
            Beep(8000, 100); /* windows.h */
            //printf("Beep sound!\n");
            chip8.registers.sound_timer -= 1;
        }
        unsigned short opcode = memory_get_short(&chip8.memory, chip8.registers.PC);
        //printf("%x\n", opcode);
        chip8.registers.PC += 2;
        exec(&chip8, opcode);
    }

out:
    SDL_DestroyWindow(window);

    return 0;
}