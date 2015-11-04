//
//  chip8.h
//  chip8
//
//  Created by Daniel Berkowitz on 4/13/15.
//  Copyright (c) 2015 Daniel Berkowitz. All rights reserved.
//

class chip8 {
public:
    chip8();
    ~chip8();
    
    bool drawFlag;
    
    void emu();
    void debugRender();
    bool loadApplication(const char * filename);
    void init();

    
    // Chip8
    unsigned char  display[64 * 32];	// Total amount of pixels: 2048
    unsigned char  keys[16];
    
private:
    unsigned short pc;				// Program counter
    unsigned short opcode;			// Current opcode
    unsigned short I;				// Index register
    unsigned short sp;				// Stack pointer
    
    unsigned char  V[16];			// V-regs (V0-VF)
    unsigned short stack[16];		// Stack (16 levels)
    unsigned char  memory[4096];	// Memory (size = 4k)
				
    unsigned char  dt;		// Delay timer
    unsigned char  st;		// Sound timer
    
};
