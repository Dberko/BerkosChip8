//
//  main.cpp
//  chip8
//
//  Created by Daniel Berkowitz on 4/13/15.
//  Copyright (c) 2015 Daniel Berkowitz. All rights reserved.
//

#include <iostream>

#include <stdio.h>
#include <glut/glut.h>
#include "chip8.h"

#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32
using namespace std;


chip8 skylark8;
int modifier = 10;

// Window size
int display_width = SCREEN_WIDTH * modifier;
int display_height = SCREEN_HEIGHT * modifier;

void reshape_window(GLsizei w, GLsizei h);
void keyboardUp(unsigned char key, int x, int y);
void keyboardDown(unsigned char key, int x, int y);
void draw_screen();
void display();


int screenData[SCREEN_WIDTH*SCREEN_HEIGHT];
void setupTexture();

int main(int argc, char * argv[]) {

    if (argc < 2) {
        printf("Usage: myChip.exe chip8app");
        return 1;
    }
    
    skylark8.init();

    if (!skylark8.loadApplication(argv[1])) {
        return 1;
    }
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    
    glutInitWindowSize(display_width, display_height);
    glutInitWindowPosition(320, 320);
    glutCreateWindow("myChip8 by Daniel Berkowitz");
    
    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutReshapeFunc(reshape_window);
    glutKeyboardFunc(keyboardDown);
    glutKeyboardUpFunc(keyboardUp);
    
    glutMainLoop();

    return 0;
}

void drawScreen() {
    for (int y = 0; y < display_height; y++) {
        for (int x = 0; x < display_width; x++) {
            if (screenData[x + (64*y)] == 0) {
                glColor3f(0.0f, 0.0f, 0.0f);
            } else {
                glColor3f(1.0f, 1.0f, 1.0f);
            }
            
            glBegin(GL_QUADS);
            
            glVertex3f((x*modifier), (y*modifier), 0.0f);
            glVertex3f((x*modifier), (y*modifier) + modifier, 0.0f);
            glVertex3f((x*modifier) + modifier, (y*modifier) + modifier, 0.0f);
            glVertex3f((x*modifier) + modifier, (y*modifier), 0.0f);
            
            glEnd();
        }
    }
    
    glutSwapBuffers();
}

void display()
{
    skylark8.emu();
    for (int i = 0; i < 32 * 64; i++) {
        screenData[i] = skylark8.display[i];
    }
    
    if(skylark8.drawFlag)
    {
        // Clear framebuffer
        glClear(GL_COLOR_BUFFER_BIT);
        
        drawScreen();
        
        // Processed frame
        skylark8.drawFlag = false;
    }
}


void reshape_window(GLsizei w, GLsizei h)
{
    glClearColor(0.0f, 0.0f, 0.5f, 0.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, h, 0);
    glMatrixMode(GL_MODELVIEW);
    glViewport(0, 0, w, h);
    
    // Resize quad
    display_width = w;
    display_height = h;
}

void keyboardDown(unsigned char keys, int x, int y)
{
    if(keys == 27)    // esc
        exit(0);
    
    if(keys == '1')		skylark8.keys[0x1] = 1;
    else if(keys == '2')	skylark8.keys[0x2] = 1;
    else if(keys == '3')	skylark8.keys[0x3] = 1;
    else if(keys == '4')	skylark8.keys[0xC] = 1;
    
    else if(keys == 'q')	skylark8.keys[0x4] = 1;
    else if(keys == 'w')	skylark8.keys[0x5] = 1;
    else if(keys == 'e')	skylark8.keys[0x6] = 1;
    else if(keys == 'r')	skylark8.keys[0xD] = 1;
    
    else if(keys == 'a')	skylark8.keys[0x7] = 1;
    else if(keys == 's')	skylark8.keys[0x8] = 1;
    else if(keys == 'd')	skylark8.keys[0x9] = 1;
    else if(keys == 'f')	skylark8.keys[0xE] = 1;
    
    else if(keys == 'z')	skylark8.keys[0xA] = 1;
    else if(keys == 'x')	skylark8.keys[0x0] = 1;
    else if(keys == 'c')	skylark8.keys[0xB] = 1;
    else if(keys == 'v')	skylark8.keys[0xF] = 1;
    
    printf("Press keys%c\n", keys);
}

void keyboardUp(unsigned char keys, int x, int y)
{
    if(keys == '1')		skylark8.keys[0x1] = 0;
    else if(keys == '2')	skylark8.keys[0x2] = 0;
    else if(keys == '3')	skylark8.keys[0x3] = 0;
    else if(keys == '4')	skylark8.keys[0xC] = 0;
    
    else if(keys == 'q')	skylark8.keys[0x4] = 0;
    else if(keys == 'w')	skylark8.keys[0x5] = 0;
    else if(keys == 'e')	skylark8.keys[0x6] = 0;
    else if(keys == 'r')	skylark8.keys[0xD] = 0;
    
    else if(keys == 'a')	skylark8.keys[0x7] = 0;
    else if(keys == 's')	skylark8.keys[0x8] = 0;
    else if(keys == 'd')	skylark8.keys[0x9] = 0;
    else if(keys == 'f')	skylark8.keys[0xE] = 0;
    
    else if(keys == 'z')	skylark8.keys[0xA] = 0;
    else if(keys == 'x')	skylark8.keys[0x0] = 0;
    else if(keys == 'c')	skylark8.keys[0xB] = 0;
    else if(keys == 'v')	skylark8.keys [0xF] = 0;
}


