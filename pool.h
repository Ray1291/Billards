#ifndef POOL_H
#define POOL_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <GL/glut.h>

#include "gltext.h"

#define GAME_WINDOW_HEIGHT 1024 
#define GAME_WINDOW_WIDTH 728

/* TABLE BASE */
#define BASE_TABLE_HEIGHT ((int) (GAME_WINDOW_HEIGHT * 0.92))
#define BASE_TABLE_WIDTH ((int)(GAME_WINDOW_WIDTH * 0.92))

#define BASE_TABLE_X ((int) ((GAME_WINDOW_WIDTH - BASE_TABLE_WIDTH) / 2))
#define BASE_TABLE_Y ((int) ((GAME_WINDOW_HEIGHT - BASE_TABLE_HEIGHT) / 2))
/************/

/* POOL TABLE */
#define POOL_TABLE_HEIGHT ((int) (GAME_WINDOW_HEIGHT * 0.80))
#define POOL_TABLE_WIDTH ((int)(GAME_WINDOW_WIDTH * 0.80))

#define POOL_TABLE_X ((int) ((GAME_WINDOW_WIDTH - POOL_TABLE_WIDTH) / 2))
#define POOL_TABLE_Y ((int) ((GAME_WINDOW_HEIGHT - POOL_TABLE_HEIGHT) / 2))
/************/

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define PI 3.1415926535
#define P2 PI/2.0
#define P3 3*PI/2.0
#define DR 0.017453
#define ONE_RADIAN_IN_DEGREES (180.0 / M_PI)

#define CLAMP_SPEED 50.0f //if velocities below this value, clamp them to 0
#define FRICTION 0.75f

/* POOL BALL */
#define CIRCLE_DEGREES 360
#define CIRCLE_RADIUS 15.0f
#define BALL_SPACING (CIRCLE_RADIUS * 2.0f)

#define NUMBER_OF_POOL_BALLS 16

#define SPACING 0.0f

#define BOTTOM_STRIP_DEGREE 45
#define TOP_STRIP_DEGREE 225

#define CUE_BALL 0
/*************/

/* POOL CUE */
#define POOL_CUE_LENGTH 400
#define POOL_CUE_WIDTH 12.0f
/*************/

/* RACK */
#define NUMBER_RACK_ROWS ((int)(ceilf(((-1 + sqrt(1 + (8 * NUMBER_OF_POOL_BALLS))) / 2))))
/*************/

/* POCKETs */
#define NUMBER_OF_POCKETS 6
#define POCKET_RADIUS 25.0f
/*************/

#define TRUE 1
#define FALSE 0

#define ARRAYLIST_CAPACITY ((NUMBER_OF_POOL_BALLS * (NUMBER_OF_POOL_BALLS - 1)) / 2)

/* KEYBOARD */
#define ESCAPE_KEY 27

struct ButtonKeys {
    int w, a, s, d, l, esc; //toggle keys
};
/*************/

typedef struct {
    GLfloat x, y;
} Point;

struct poolBall {
    GLfloat radius;
    int color;
    int number; //0 - 12
    int striped; // 0 or 1
    int visible;
    int id; 
    GLfloat pos_x, pos_y;
    GLfloat vel_x, vel_y;
    GLfloat acc_x, acc_y;
    GLfloat mass;
};

struct pocketHole {
    GLfloat radius;
    int visible;
    int id; 
    GLfloat pos_x, pos_y;
};

struct MouseData {
    int pressed_mouse_x;
    int pressed_mouse_y;
    int curr_mouse_x;
    int curr_mouse_y;
    int mouse_pressed;
};

struct RackPositions {
    int x1, y1;
    int width, height;
};

struct poolBallPair {
    struct poolBall *first;
    struct poolBall *second;
};

typedef struct {
    struct poolBallPair *data;
    size_t size;
    size_t capacity;
} ArrayListPairs;

extern GLfloat pool_ball_colors[NUMBER_OF_POOL_BALLS][3];
extern struct poolBall pool_balls_array[NUMBER_OF_POOL_BALLS];
extern struct pocketHole pool_pockets_array[NUMBER_OF_POCKETS];

extern struct ButtonKeys Keys;
extern struct MouseData Mouse;
extern struct RackPositions Rack;

extern ArrayListPairs colliding_pairs;

extern struct poolBall *selected_ball;

extern float player_x_position, player_y_position;
extern float frame1;

extern int can_selection_printed;

extern int score;

extern unsigned int texture_background;
extern unsigned int texture_pool_cue;
extern unsigned int texture_pool_table;
extern unsigned int texture_pool_rack;
extern unsigned int texture_pool_base;

extern GLTtext *text;

#endif