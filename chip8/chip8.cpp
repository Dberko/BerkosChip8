//
//  chip8.cpp
//  chip8
//
//  Created by Daniel Berkowitz on 4/13/15.
//  Copyright (c) 2015 Daniel Berkowitz. All rights reserved.
//

#include "chip8.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


unsigned char memory[4096];
char v[16]; // Vx register
unsigned short stack[16];
unsigned short sp; // stack pointer
int display[32 * 64]; // 1 = pixel, 0 = none
int pc;
unsigned char keys[16];
bool drawFlag = 0; // 0 = don't draw, 1 = draw
int I;
int opcode;
unsigned char dt; // delay timer
unsigned char st; // sound timer

unsigned char chip8_fontset[80] =
{
    0xF0, 0x90, 0x90, 0x90, 0xF0, //0
    0x20, 0x60, 0x20, 0x20, 0x70, //1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
    0x90, 0x90, 0xF0, 0x10, 0x10, //4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
    0xF0, 0x10, 0x20, 0x40, 0x40, //7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
    0xF0, 0x90, 0xF0, 0x90, 0x90, //A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
    0xF0, 0x80, 0x80, 0x80, 0xF0, //C
    0xE0, 0x90, 0x90, 0x90, 0xE0, //D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
    0xF0, 0x80, 0xF0, 0x80, 0x80  //F
};

chip8::chip8() {

}

chip8::~chip8() {

}

void chip8::init() {
    pc     = 0x200;  // Program counter starts at 0x200
    opcode = 0;      // Reset current opcode
    I      = 0;      // Reset index register
    sp     = 0;      // Reset stack pointer
    
    // Clear display
    for(int i = 0; i < 2048; ++i)
        gfx[i] = 0;
    
    // Clear stack
    for(int i = 0; i < 16; ++i)
        stack[i] = 0;
    
    for(int i = 0; i < 16; ++i)
        key[i] = V[i] = 0;
    
    // Clear memory
    for(int i = 0; i < 4096; ++i)
        memory[i] = 0;
    
    // Load fontset
    for(int i = 0; i < 80; ++i)
        memory[i] = chip8_fontset[i];		
    
    dt = st = 0;
    drawFlag = true;
    srand(time(NULL));
    
}


void chip8::emu() {
    unsigned short height;
    unsigned short pixel;
    int keyPress = 0;

    pc = 0x200;
    opcode = memory[pc] << 8 | memory[pc + 1];

    switch (opcode & 0xF000) {
            // Clear screen
        case 0x000:
            switch (opcode & 0x000F) {
                case 0x0000:
                    for (int a = 0; a < 32*64; a++) {
                        display[a] = 0;
                    }
                    drawFlag = true;
                    break;
            }
            break;
            
            // Return from a subroutine
        {
        case 0x000E:
            sp--; // pop stack.
            pc = stack[sp];
            pc += 2; // 2 bytes each command.
            break;
        }
            
            // Jump to nnn
        {
        case 0x1000:
            pc = opcode & 0x0FFF;
            break;
        }
            
            // Call subroutine at nnn
        {
        case 0x2000:
            stack[sp] = pc;
            sp++;
            pc = opcode & 0x0FFF;
            break;
        }
            
            // 3xKK if Vx == kk then skip step
        {
        case 0x3000:
            if (v[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)) {
                pc += 4;
            } else {
                pc += 2;
            }
            break;
        }
            
        {
        case 0x4000:
            if (v[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)) {
                pc += 4;
            } else {
                pc += 2;
                
            }
        }
            
        {
        case 0x5000:
            if (v[(opcode & 0x0F00) >> 8] == v[(opcode & 0x00F0) >> 4]) {
                pc += 4;
            } else {
                pc += 2;
            }
            break;
        }
        {
        case 0x6000:
            v[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
            pc += 2;
            break;
        }
        {
        case 0x7000:
            v[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
            pc += 2;
            break;
        }
        {
        case 0x08000:
            switch (opcode & 0x000F) {
                case 0x0000:
                    v[(opcode & 0x0F00) >> 8] = v[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                case 0x0001:
                    v[(opcode & 0x0F00) >> 8] |= v[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                case 0x0002:
                    v[(opcode & 0x0F00) >> 8] &= v[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                case 0x0003:
                    v[(opcode & 0x0F00) >> 8] ^= v[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                    // Set Vx = Vx + Vy, set VF = carry.
                case 0x0004:
                    v[(opcode & 0x0F00) >> 8] += v[(opcode & 0x00F0) >> 4];
                    if(v[(opcode & 0x00F0) >> 4] >
                       (0xFF - v[(opcode & 0x0F00) >> 8])) {
                        v[0xF] = 1;
                    } else {
                        v[0xF] = 0;
                    }
                    pc += 2;
                    break;
                    // Vx = Vx - Vy
                case 0x0005:
                    v[(opcode & 0x0F00) >> 8] -= v[(opcode & 0x00F0) >> 4];
                    if(v[(opcode & 0x00F0) >> 4] >
                       (0xFF - v[(opcode & 0x0F00) >> 8])) {
                        v[0xF] = 0;
                    }
                    else  {
                        v[0xF] = 1;
                    }
                    pc += 2;
                    break;
                case 0x0006:
                    v[0xF] = v[(opcode & 0x0F00) >> 8] & 0x1;
                    v[(opcode & 0x0F00) >> 8] >>= 1;
                    pc += 2;
                    break;
                case 0x0007:
                    if(v[(opcode & 0x00F0) >> 4] < v[(opcode & 0x0F00) >> 8]) {
                        v[0xF] = 1;
                    } else {
                        v[0xF] = 0;
                    }
                    v[(opcode & 0x0F00) >> 8] = v[(opcode & 0x00F0) >> 4] -
                    v[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;
                    // Set Vx = Vx SHL 1.
                case 0x000E:
                    v[0xF] = v[(opcode & 0x0F00) >> 8] >> 7;
                    v[(opcode & 0x0F00) >> 8] <<= 1;
                    pc += 2;
                    break;
                    
            }
        }
        {
        case 0x9000:
            if (v[(opcode & 0x00F0) >> 4] != v[(opcode & 0x0F00) >> 8]) {
                pc += 4;
            } else {
                pc += 4;
            }
            break;
        }
            
            // Annn Register I = nnn
        {
        case 0xA000:
            I = opcode & 0x0FFF;
            pc += 2;
            break;
        }
            
            // Bnnn jump to location nnn + v0
        {
        case 0xB000:
            pc = (opcode & 0x0FFF) + v[0];
            break;
        }
            
            //Cxkk Vx = random byte and kk
        {
        case 0xC000:
            v[(opcode & 0x0F00) >> 8] = (rand() % 0xFF) & (opcode & 0x00FF);
            pc += 2;
            break;
        }
            
            // DRW Vx, Vy, nibble
        {
        case 0xD000:
            unsigned short x = v[(opcode & 0x0F00) >> 8];
            unsigned short y = v[(opcode & 0x00F0) >> 4];
            height = opcode & 0x000F;
            v[0xF] = 0;
            for (int a = 0; a < height; a++) {
                pixel = memory[I + a];
                for (int b = 0; b < 8; b++) {
                    if ((pixel & (0x80 >> b)) != 0) {
                        if (display[(x + b + ((y + a) * 64))] == 1) {
                            v[0xF] = 1;
                        }
                        display[(x + b + ((y + a) * 64))] ^= 1;
                    }
                }
            }
            drawFlag = true;
            
            pc += 2;
            break;
        }
        {
        case 0xE000:
            {
                switch (opcode & 0x00FF) {
                    case 0x009E:
                        if (keys[v[(opcode & 0x0F00) >> 8]] != 0) {
                            pc += 4;
                        } else {
                            pc += 2;
                        }
                        break;
                    case 0x00A1:
                        if (keys[v[(opcode & 0x0F00) >> 8]] == 0) {
                            pc += 4;
                            
                        } else {
                            pc += 2;
                        }
                        break;
                        
                }
            }
            
        }
        {
        case 0xF000:
            {
                switch (opcode & 0x00FF) {
                    case 0x0007:
                        v[(opcode & 0x0F00) >> 8] = dt;
                        pc += 2;
                        break;
                    case 0x000A:
                        keyPress = 0;
                        for (int a = 0; a < 16; a++) {
                            if (keys[a] != 0) {
                                v[(opcode & 0x0F00) >> 8] = a;
                                keyPress = 1;
                            }
                        }
                        pc += 2;
                        break;
                    case 0x0015:
                        dt = v[(opcode & 0x0F00) >> 8];
                        pc += 2;
                        break;
                        // Set sound timer = Vx.
                    case 0x0018:
                        st = v[(opcode & 0x0F00) >> 8];
                        pc += 2;
                        break;
                        // Set I = I + Vx.
                    case 0x001E:
                        I += v[(opcode & 0x0F00) >> 8];
                        if(I > 0xFFF) {
                            v[0xF] = 1;
                        }
                        else {
                            v[0xF] = 0;
                        }
                        pc += 2;
                        break;
                        // Set I = location of sprite for digit Vx.
                    case 0x0029:
                        I = v[(opcode & 0x0F00) >> 8] * 0x5;
                        pc += 2;
                        break;
                        //Fx33 Store BCD representations of Vx in memory
                    case 0x0033:
                        memory[I] = v[(opcode & 0x0F00) >> 8] / 100;
                        memory[I + 1] = (v[(opcode & 0x0F00) >> 8] / 10) % 10;
                        memory[I + 2] = (v[(opcode & 0x0F00) >> 8] % 100) % 10;
                        pc += 2;
                        break;
                        //Fx55 Stores V0 to VX in memory starting at address I
                    case 0x0055:
                        for (int a = 0; a <= ((opcode & 0x0F00) >> 8); a++) {
                            memory[I + a] = v[a];
                        }
                        I += ((opcode & 0x0F00) >> 8) + 1;
                        pc += 2;
                        break;
                        
                    case 0x0065:
                        for (int a = 0; a <= ((opcode & 0x0F00) >> 8); a++)
                        {
                            v[a] = memory[I + a];
                        }
                        I += ((opcode & 0x0F00) >> 8) + 1;
                        pc += 2;
                        break;
                }
                
                
            }
        default:
            printf ("Unknown opcode: 0x%X\n", opcode);
            break;
            
            
        }
            

    }
    
    if (dt > 0) {
        dt--;
    }
    
    if (st > 0) {
        st--;
    }
    
}

void chip8::debugRender() {
    for (int y = 0; y < 32; y++) {
        for (int x = 0; x < 64; x++) {
            if (display[(y*64) + x] == 0) {
                printf("0");
            } else {
                printf(" ");
            }
        }
        printf("\n");
    }
    printf("\n");
}

bool chip8::loadApplication(const char * filename) {
    init();
    printf("Loading: %s\n", filename);
    FILE * pFile = fopen(filename, "rb");
    if (pFile == NULL) {
        printf("FILE ERROR", stderr);
        return false;
    }
    
    fseek(pFile, 0, SEEK_END);
    long lsize = ftell(pFile);
    rewind(pFile);
    printf("Filesize: %d\n", (int) lsize);
    
    // Allocate memory to contain the whole file
    char * buffer = (char*)malloc(sizeof(char) * lsize);
    if (buffer == NULL)
    {
        fputs ("Memory error", stderr);
        return false;
    }
    
    // Copy the file into the buffer
    size_t result = fread (buffer, 1, lsize, pFile);
    if (result != lsize)
    {
        fputs("Reading error",stderr);
        return false;
    }
    
    // Copy buffer to Chip8 memory
    if((4096-512) > lsize)
    {
        for(int i = 0; i < lsize; ++i)
            memory[i + 512] = buffer[i];
    }
    else
        printf("Error: ROM too big for memory");
    
    // Close file, free buffer
    fclose(pFile);
    free(buffer);
    
    return true;
    
    

}
