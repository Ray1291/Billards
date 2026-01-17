#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "pool.h"
#include "movement_handling.h"

void movement_handler(float deltaTime){
    if (Keys.w == 1){
        player_y_position -= 100.0f * deltaTime;
    }

    if (Keys.s == 1){
        player_y_position += 100.0f * deltaTime;
    }

    if (Keys.a == 1){
        player_x_position -= 100.0f * deltaTime;}

    if (Keys.d == 1){
        player_x_position += 100.0f * deltaTime;
    }
}

void ButtonDown(unsigned char key, int x, int y){
    if (key == 'a') {Keys.a = 1;}
    if (key == 'd') {Keys.d = 1;}
    if (key == 's') {Keys.s = 1;}
    if (key == 'w') {Keys.w = 1;}
    if (key == ESCAPE_KEY) {Keys.esc = 1;}
}

void ButtonUp(unsigned char key, int x, int y){
    if (key == 'a') {Keys.a = 0;}
    if (key == 'd') {Keys.d = 0;}
    if (key == 's') {Keys.s = 0;}
    if (key == 'w') {Keys.w = 0;}
    if (key == ESCAPE_KEY) {Keys.esc = 0;}
}

void mouse_button_handler(int button, int state, int x, int y){
    if(button == GLUT_LEFT_BUTTON ){
        if(state == GLUT_DOWN){
            Mouse.mouse_pressed = TRUE;
            Mouse.pressed_mouse_x = x;
            Mouse.pressed_mouse_y = y;
            
            printf("Mouse just pressed at (%d, %d) state: %d\n", x, y, Mouse.mouse_pressed);
            fflush(stdout);
        }
        else if (state == GLUT_UP){
            Mouse.mouse_pressed = FALSE;

            printf("Mouse just release at (%d, %d) state: %d\n", x, y, Mouse.mouse_pressed);
            fflush(stdout);
        }
    }    
}

void mouse_movement(int x, int y){
    Mouse.curr_mouse_x = x;
    Mouse.curr_mouse_y = y;
    //printf("mouse poss : (%d, %d), state: %d, last: %d\n", x, y, mouse_pressed, last_mouse_state);
    glutPostRedisplay();
}