#include "chip8.h"
#include <memory.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include "SDL2/SDL.h"

const char default_character_set[] = {
    0xf0, 0x90, 0x90, 0x90, 0xf0,
    0x20, 0x60, 0x20, 0x20, 0x70,
    0xf0, 0x10, 0xf0, 0x80, 0xf0,
    0xf0, 0x10, 0xf0, 0x10, 0xf0,
    0x90, 0x90, 0xf0, 0x10, 0x10,
    0xf0, 0x80, 0xf0, 0x10, 0xf0,
    0xf0, 0x80, 0xf0, 0x90, 0xf0,
    0xf0, 0x10, 0x20, 0x40, 0x40,
    0xf0, 0x90, 0xf0, 0x90, 0xf0,
    0xf0, 0x90, 0xf0, 0x10, 0xf0,
    0xf0, 0x90, 0xf0, 0x90, 0x90,
    0xe0, 0x90, 0xe0, 0x90, 0xe0,
    0xf0, 0x80, 0x80, 0x80, 0xf0,
    0xe0, 0x90, 0x90, 0x90, 0xe0,
    0xf0, 0x80, 0xf0, 0x80, 0xf0,
    0xf0, 0x80, 0xf0, 0x80, 0x80
};

void init(struct chip8* chip8){
    memset(chip8, 0, sizeof(struct chip8));
    memcpy(&chip8->memory.memory, default_character_set, sizeof(default_character_set));
}

void load(struct chip8* chip8, const char* buf, size_t size){
    assert(size + PROGRAM_LOAD_ADDRESS < MEMORY_SIZE);
    memcpy(&chip8->memory.memory[PROGRAM_LOAD_ADDRESS], buf, size);
    chip8->registers.PC = PROGRAM_LOAD_ADDRESS;
}

static void exec_extended_eight(struct chip8* chip8, unsigned short opcode){
    unsigned char x = (opcode >> 8) & 0x000f;
    unsigned char y = (opcode >> 4) & 0x000f;
    unsigned char final_fourbits = opcode & 0x000f;
    int tmp = 0;
    switch(final_fourbits){
        // 8xy0 - LD Vx, Vy. Vx = Vy
        case 0x00:
            chip8->registers.V[x] = chip8->registers.V[y];
        break;

        // 8xy1 - OR Vx, Vy. Performs a bitwise OR on Vx and Vy stores the result in Vx
        case 0x01:
            chip8->registers.V[x] |= chip8->registers.V[y]; 
        break;

        // 8xy2 - AND Vx, Vy. Performs a bitwise AND on Vx and Vy stores the result in Vx
        case 0x02:
            chip8->registers.V[x] &= chip8->registers.V[y]; 
        break;

        // 8xy3 - XOR Vx, Vy. Performs a bitwise AND on Vx and Vy stores the result in Vx
        case 0x03:
            chip8->registers.V[x] ^= chip8->registers.V[y]; 
        break;

        // 8xy4 - ADD Vx, Vy. Set Vx = Vx + Vy, set VF = carry
        case 0x04:
            tmp = chip8->registers.V[x] + chip8->registers.V[y];
            chip8->registers.V[0x0f] = (tmp > 0xff);
            chip8->registers.V[x] = tmp;
        break;

        // 8xy5 - SUB Vx, Vy. Set Vx = Vx - Vy, set VF = Not borrow
        case 0x05:
            if (chip8->registers.V[x] > chip8->registers.V[y]){
                chip8->registers.V[0x0f] = 1;
            }
            chip8->registers.V[x] -= chip8->registers.V[y];
        break;

        // 8xy6 - SHR Vx {, Vy}, Set Vx = Vx SHR 1
        case 0x06:
            chip8->registers.V[0x0f] = chip8->registers.V[x] & 0x01;
            chip8->registers.V[x] = chip8->registers.V[x] >> 1;
        break;
        
        // 8xy7 - SUBN Vx, Vy. Vx = Vy - Vx, VF = NOT borrow
        case 0x07:
            chip8->registers.V[0x0f] = chip8->registers.V[y] > chip8->registers.V[x];
            chip8->registers.V[x] = chip8->registers.V[y] - chip8->registers.V[x];
        break;

        // 8xyE - SHL Vx {, Vy}
        case 0x0E:
            chip8->registers.V[0x0f] = ((chip8->registers.V[y] & 0x80) == 0x80);
            chip8->registers.V[x] = chip8->registers.V[x] << 1;
        break;
    }
    
}

static char wait_for_key_press(struct chip8* chip8){
    SDL_Event event;
    while(SDL_WaitEvent(&event)){
        if (event.type != SDL_KEYDOWN){
            continue;
        }

        char c = event.key.keysym.sym;
        char key = keyboard_map(chip8->keyboard.keyboard_map, c);
        if (key != -1){
            return key;
        }
    }
    return -1; /* unrecheable */
}

static void exec_extended_F(struct chip8* chip8, unsigned short opcode){
    unsigned char x = (opcode >> 8) & 0x000f;
    switch(opcode & 0x00ff){
        // fx07 - LD Vx, DT. Set Vx to the delay timer value.
        case 0x07:
            chip8->registers.V[x] = chip8->registers.delay_timer;
        break;

        // fx0a - LD Vx, k
        case 0x0A:{
            char pressed_key = wait_for_key_press(chip8);
            chip8->registers.V[x] = pressed_key;
        }
        break;

        // fx15 - LD DT, Vx. set the delay timer to Vx
        case 0x15:
            chip8->registers.delay_timer = chip8->registers.V[x];
        break;

        // fx18 - LD ST, Vx. set the sounde timer to Vx
        case 0x18:
            chip8->registers.sound_timer = chip8->registers.V[x];
        break;

        // fx1e - ADD I, Vx
        case 0x1E:
            chip8->registers.I += chip8->registers.V[x];
        break;

        // fx29 - LD F, Vx
        case 0x29:
            chip8->registers.I = chip8->registers.V[x] * DEFAULT_SPRITE_HEIGHT;
        break;

        // fx33 - LD B, Vx
        case 0x33:{
            unsigned char hundreds = chip8->registers.V[x] / 100 % 10;
            unsigned char tens     = chip8->registers.V[x] / 10 % 10;
            unsigned char units    = chip8->registers.V[x] % 10;

            memory_set(&chip8->memory, chip8->registers.I    , hundreds);
            memory_set(&chip8->memory, chip8->registers.I + 1, tens);
            memory_set(&chip8->memory, chip8->registers.I + 2, units);
        }
        break;

        // fx55 - LD [I], Vx
        case 0x55:{
             for(int i = 0; i < x; i++){
                 memory_set(&chip8->memory, chip8->registers.I + i, chip8->registers.V[i]);
             }
        }
        break;

        // fx65 - LD Vx, [I]
        case 0x65:{
            for(int i = 0; i <= x; i++){
                chip8->registers.V[i] = memory_get(&chip8->memory, chip8->registers.I + i);
            }
        }
        break;
    }
}

static void exec_extended(struct chip8* chip8, unsigned short opcode){

    unsigned short nnn = opcode & 0x0fff;
    unsigned char x = (opcode >> 8) & 0x000f;
    unsigned char y = (opcode >> 4) & 0x000f;
    unsigned char kk = opcode & 0x00ff;
    unsigned char n = opcode & 0x000f;

    switch(opcode & 0xf000){
        // JP addr, 1nnn jump to location nnn.
        case 0x1000:
            chip8->registers.PC = nnn;
        break;

        // Call addr, Call subroutine at location nnn
        case 0x2000:
            stack_push(chip8, chip8->registers.PC);
            chip8->registers.PC = nnn;
        break;

        // SE Vx, byte - 3xkk Skip next instruction if Vx == kk 
        case 0x3000:
            if (chip8->registers.V[x] == kk){
                chip8->registers.PC += 2;
            }
        break;
        
        // SNE Vx, byte - 4xkk Skip next instruction if Vx != kk 
        case 0x4000:
            if (chip8->registers.V[x] != kk){
                chip8->registers.PC += 2;
            }
        break;

        // SE Vx, Vy - 5xy0 Skip next instruction if Vx == Vy
        case 0x5000:
            if (chip8->registers.V[x] == chip8->registers.V[y]){
                chip8->registers.PC += 2;
            }
        break;

        // 6xkk - LD Vx, Byte, Vx = kk
        case 0x6000:
            chip8->registers.V[x] = kk;
        break;
        
        // 7xkk - ADD Vx, byte. Set Vx = Vx + kk        
        case 0x7000:
            chip8->registers.V[x] += kk;
        break;

        case 0x8000:
            exec_extended_eight(chip8, opcode);
        break;

        // 9xy0 - SNE Vx, Vy. Skip next instruction if Vx != Vy
        case 0x9000:
            if (chip8->registers.V[x] != chip8->registers.V[y]){
                chip8->registers.PC += 2;
            }
        break;

        // Annn LD 1, addr
        case 0xA000:
            chip8->registers.I = nnn;
        break;

        // Bnnn JP V0, addr
        case 0xB000:
            chip8->registers.PC = nnn + chip8->registers.V[0x00];
        break;

        // Cxkk: RND Vx, byte
        case 0xC000:
            srand(clock());
            //chip8->registers.V[x] = (rand() % 255) & kk;
            chip8->registers.V[x] = rand() & kk;
        break;

        // Dxyn - DRW Vx, Vy, nibble. Draws sprite to the screen
        case 0xD000:{
            const char* sprite = (const char*)&chip8->memory.memory[chip8->registers.I];
            chip8->registers.V[0x0f] = screen_draw_sprite(&chip8->screen, chip8->registers.V[x], chip8->registers.V[y], sprite, n);
        }
        break;

        // Keyboard Operations
        case 0xE000:{
            switch(opcode & 0x00ff){
                // Ex9e - SKP Vx, Skip the next instruction if the key with the value of Vx is pressed
                case 0x9e:
                    if (keyboard_is_down(&chip8->keyboard, chip8->registers.V[x])){
                        chip8->registers.PC += 2;
                    }
                break;

                // Exa1 - SKNP Vx - Skip the next instruction if the key with the value of Vx is not pressed
                case 0xa1:
                    if (!keyboard_is_down(&chip8->keyboard, chip8->registers.V[x])){
                        chip8->registers.PC += 2;
                    }
                break;
            }
        }
        break;

        case 0xF000:
            exec_extended_F(chip8, opcode);
        break;
    }
}

void exec(struct chip8* chip8, unsigned short opcode){
    switch (opcode)
    {
        //CLS: Clear The Display
        case 0x00E0:
            screen_clear(&chip8->screen);
        break;

        //RET: Return From Subroutine
        case 0x00EE:
            chip8->registers.PC = stack_pop(chip8);
        break;

        default:
            exec_extended(chip8, opcode);
        break;
    }

}