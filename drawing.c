#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// #define GLT_IMPLEMENTATION
// #include "gltext.h"

// #include "gltext.h"

#include "pool.h"
#include "arraylist.h"

extern unsigned int texture_background;
extern unsigned int texture_pool_cue;
extern unsigned int texture_pool_table;

int find_distance(float x1, float y1, float x2, float y2){
    return sqrtf((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

float random_float_range(float min, float max){
    float scale = (float) rand() / (float) RAND_MAX;
    return min + scale * (max - min);
}

float degree_to_radian(int i){
    return i * (PI / 180);
}

float radian_to_degree(float i){
    return i * 180 / PI;
}

// void write_text(const char *string, Point p){
//     glEnable(GL_BLEND);
//     glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//     gltSetText(text, string);
//     gltBeginDraw();
//     gltColor(1.0f, 1.0f, 1.0f, 1.0f);
//     gltDrawText2D(text, p.x, p.y, 1.0f);
//     gltEndDraw();

//     glUseProgram(0);            // Unbind GLText shader
//     glBindVertexArray(0);       // Reset VAO
//     glBindBuffer(GL_ARRAY_BUFFER, 0); // Reset VBO
//     glColor3f(1.0f, 1.0f, 1.0f);      // Reset fixed-function color
// }

unsigned int load_texture(char image_name[]){
    printf("LOADING Texture: %s\n", image_name);
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, texture);
    stbi_set_flip_vertically_on_load(TRUE);

    int image_width, image_height, nrChannels;
    unsigned char *texture_file = stbi_load(image_name, &image_width, &image_height, &nrChannels, 0);

    if (texture_file != NULL)
    {
        GLenum format = GL_RGB;
        if (nrChannels == 4)
        {
            format = GL_RGBA;
        }
        else if (nrChannels == 3)
        {
            format = GL_RGB;
        }

        //printf("NUMBER OF CHANNELS: %d\n", nrChannels);
        glTexImage2D(GL_TEXTURE_2D, 0, format, image_width, image_height, 0, format, GL_UNSIGNED_BYTE, texture_file);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
    else
    {
        printf("FAILED TO LOAD TEXTURE");
    }

    stbi_image_free(texture_file);
    printf("LOADED Texture: %s\n", image_name);
    return texture;
}

void draw_line(Point p1, Point p2){
    glBegin(GL_LINES);
    glColor3f(1.0, 0.0, 0.0);
    glVertex2f(p1.x, p1.y);
    glVertex2f(p2.x, p2.y);
    glEnd(); 
}

void draw_element(unsigned int texture, Point p1, Point p2, Point p3, Point p4){
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glColor4f(1.0, 1.0, 1.0, 1.0);
    
    glDisable(GL_DEPTH_TEST);

    glDepthMask(GL_FALSE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_TEXTURE_2D);    
    glBindTexture(GL_TEXTURE_2D, texture);

    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(p1.x, p1.y);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(p2.x, p2.y);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(p3.x, p3.y);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(p4.x, p4.y);
    glEnd();

    glEnd(); 

    glDisable(GL_TEXTURE_2D);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
}

/////////////////////////
void draw_circle_3channel(GLint start, GLint degrees, GLfloat x, GLfloat y, GLfloat radius, GLfloat red, GLfloat green, GLfloat blue){
    GLint i;
    float angle_theta;

    glBegin(GL_TRIANGLE_FAN);
    glColor3f(red, green, blue);
    glVertex2f(x, y);
    // printf("RADIUS: %f\n", pocket->radius);
    // fflush(stdout);

    for (i = start; i <= degrees; i++){
        angle_theta = degree_to_radian(i);

        glVertex2f(
            x + (radius * cosf(angle_theta)),
            y + (radius * sinf(angle_theta))
        );
    }
    glEnd();
}

void draw_stripes(GLint start, GLint end, GLfloat x, GLfloat y, GLfloat radius){
    GLint i;
    float angle_theta;
    
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(1.0, 1.0, 1.0);

    glVertex2f(
        x + (radius * cosf(degree_to_radian(start))),
        y + (radius * sinf(degree_to_radian(start + 90)))
    );

    for (i = start; i <= end; i++) {
        angle_theta = degree_to_radian(i);
        glVertex2f(
            x + (radius * cosf(angle_theta)),
            y + (radius * sinf(angle_theta)));
    }
    glEnd();
}

void draw_pockets(GLint degrees, struct pocketHole *pocket){
    draw_circle_3channel(0, degrees, pocket->pos_x, pocket->pos_y, pocket->radius, 0.1f, 0.1f, 0.1f);
}

void draw_pool_ball(GLint degrees, struct poolBall *ball){
    GLint i;
    float angle_theta;

    draw_circle_3channel(0, degrees, ball->pos_x, ball->pos_y, ball->radius, pool_ball_colors[ball->color][0], pool_ball_colors[ball->color][1], pool_ball_colors[ball->color][2]);
    draw_circle_3channel(0, degrees, ball->pos_x, ball->pos_y, ball->radius / 2, 1.0f, 1.0f, 1.0f);

    /*
        CHECK IF BALL STRIPED
        If it is, calculate the x and y values to place the stripe.
        Strip start degrees = 45, 225
        Strip end degrees = 135, 315
    */
    if (ball->striped == TRUE) {
        draw_stripes(BOTTOM_STRIP_DEGREE, BOTTOM_STRIP_DEGREE + 90, ball->pos_x, ball->pos_y, ball->radius);
        draw_stripes(TOP_STRIP_DEGREE, TOP_STRIP_DEGREE + 90, ball->pos_x, ball->pos_y, ball->radius);
    }
}
///////////////////////////////////////

void draw_pool_cue(unsigned int texture) {
    Point p1, p2, p3, p4;
    float cue_angle = atan2(Mouse.curr_mouse_y - selected_ball->pos_y, Mouse.curr_mouse_x - selected_ball->pos_x);

    // printf("The angle is %f\n", radian_to_degree(cue_angle));
    fflush(stdout);

    float x_component = cosf(cue_angle);
    float y_component = sinf(cue_angle);

    // glVertex2f(
    //     ball->pos_x + ((ball->radius /2) * cosf(angle_theta)),
    //     ball->pos_y + ((ball->radius /2) * sinf(angle_theta))
    // );

    float x_normal = -y_component;
    float y_normal = x_component;

    float half_width = POOL_CUE_WIDTH * 0.5;
    float distance = find_distance(Mouse.curr_mouse_x, Mouse.curr_mouse_y, selected_ball->pos_x, selected_ball->pos_y);
    
    if(distance == 0.0){
        return;
    }
    //float distance = find_distance(curr_mouse_x, curr_mouse_y, selected_ball->pos_x, selected_ball->pos_y);

    float x_0 = (selected_ball->pos_x) + (selected_ball->radius * x_component * (distance * 0.05f));
    float y_0 = (selected_ball->pos_y) + (selected_ball->radius * y_component * (distance * 0.05f));

    float x1 = x_0 + x_component * POOL_CUE_LENGTH;
    float y1 = y_0 + y_component * POOL_CUE_LENGTH;

    p1.x = x_0 + x_normal * half_width;
    p1.y = y_0 + y_normal * half_width;

    p2.x = x1 + x_normal * half_width;
    p2.y = y1 + y_normal * half_width;

    p3.x = x1 - x_normal * half_width;
    p3.y = y1 - y_normal * half_width;

    p4.x = x_0 - x_normal * half_width;
    p4.y = y_0 - y_normal * half_width;

    draw_element(texture, p1, p2, p3, p4); //drawing pool cue

    draw_line(
        (Point){.x = selected_ball->pos_x, .y = selected_ball->pos_y}, 
        (Point){.x = Mouse.curr_mouse_x, .y = Mouse.curr_mouse_y}
    );     

    // Point hit = cast_ray();

    // draw_line(
    //     (Point){.x = reflected_x, .y = reflected_y}, 
    //     (Point){.x = selected_ball->pos_x, .y = selected_ball->pos_y}
    // );     

    // char power[8];
    //snprintf(power, sizeof(power), "%.2f", distance);
    // write_text(strcat("Power", power), (Point) {.x = 0, .y = 0});
}

void draw_background(){
    draw_element(texture_background, 
        (Point){.x = 0, .y = 0}, 
        (Point){.x = GAME_WINDOW_WIDTH, .y = 0},
        (Point){.x = GAME_WINDOW_WIDTH, .y = GAME_WINDOW_HEIGHT},
        (Point){.x = 0, .y = GAME_WINDOW_HEIGHT}
    );

    draw_element(texture_pool_base, 
        (Point){.x = BASE_TABLE_X, .y = BASE_TABLE_Y}, 
        (Point){.x = BASE_TABLE_X + BASE_TABLE_WIDTH, .y = BASE_TABLE_Y},
        (Point){.x = BASE_TABLE_X + BASE_TABLE_WIDTH, .y = BASE_TABLE_Y + BASE_TABLE_HEIGHT},
        (Point){.x = BASE_TABLE_X, .y = BASE_TABLE_Y + BASE_TABLE_HEIGHT}
    );

    draw_element(texture_pool_table, 
        (Point){.x = POOL_TABLE_X, .y = POOL_TABLE_Y}, 
        (Point){.x = POOL_TABLE_X + POOL_TABLE_WIDTH, .y = POOL_TABLE_Y},
        (Point){.x = POOL_TABLE_X + POOL_TABLE_WIDTH, .y = POOL_TABLE_Y + POOL_TABLE_HEIGHT},
        (Point){.x = POOL_TABLE_X, .y = POOL_TABLE_Y + POOL_TABLE_HEIGHT}
    );
}


