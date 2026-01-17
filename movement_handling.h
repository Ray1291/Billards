#ifndef MOVEMENT_HANDLING_H
#define MOVEMENT_HANDLING_H

// Input handlers
void movement_handler(float deltaTime);
void ButtonDown(unsigned char key, int x, int y);
void ButtonUp(unsigned char key, int x, int y);
void mouse_button_handler(int button, int state, int x, int y);
void mouse_movement(int x, int y);

#endif