#ifndef DRAWING_H
#define DRAWING_H

#include "pool.h"
#include <GL/glut.h>

//Math
float find_distance(float, float, float, float);
float degree_to_radian(int);
float radian_to_degree(float);
float random_float_range(float, float);

//Text Writing
// void write_text(const char *string, Point p);

//Texture Loading
unsigned int load_texture(char texture_name[]);

//Basic Shape Drawing
void draw_circle_3channel(GLint start, GLint degrees, GLfloat x, GLfloat y, GLfloat radius, GLfloat red, GLfloat green, GLfloat blue);
void draw_pockets(GLint, struct pocketHole *);
void draw_line(Point p1, Point p2); 

//Element Drawing
void draw_element(unsigned int, Point, Point, Point, Point);
void draw_pool_ball(GLint, struct poolBall *);
void draw_pool_cue();
void draw_background();

#endif