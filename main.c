#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "pool.h"
#include "drawing.h"
#include "arraylist.h"
#include "collisions.h"
#include "movement_handling.h"

// gcc main.c helpers.c drawing.c -o a.out -lglut -lGLEW -lGL -lGLU -lm -g

GLfloat pool_ball_colors[NUMBER_OF_POOL_BALLS][3] = {
    {1.0f, 1.0f, 1.0f},   /* 0: Cue ball */
    {1.0f, 1.0f, 0.0f},   /* 1: Yellow */
    {0.0f, 0.0f, 1.0f},   /* 2: Blue */
    {1.0f, 0.0f, 0.0f},   /* 3: Red */
    {0.5f, 0.0f, 0.5f},   /* 4: Purple */
    {1.0f, 0.5f, 0.0f},   /* 5: Orange */
    {0.0f, 0.6f, 0.0f},   /* 6: Green */
    {0.5f, 0.0f, 0.0f},   /* 7: Maroon */
    {0.0f, 0.0f, 0.0f},   /* 8: Eight ball */
    {1.0f, 1.0f, 0.0f},   /* 9: Yellow stripe */
    {0.0f, 0.0f, 1.0f},   /* 10: Blue stripe */
    {1.0f, 0.0f, 0.0f},   /* 11: Red stripe */
    {0.5f, 0.0f, 0.5f},   /* 12: Purple stripe */
    {1.0f, 0.5f, 0.0f},   /* 13: Orange stripe */
    {0.0f, 0.6f, 0.0f},   /* 14: Green stripe */
    {0.5f, 0.0f, 0.0f}    /* 15: Maroon stripe */
};

float player_x_position, player_y_position;
float frame1 = 0;

int can_selection_printed = FALSE;
int score = 0;

unsigned int texture_background;
unsigned int texture_pool_cue;
unsigned int texture_pool_table;
unsigned int texture_pool_rack;
unsigned int texture_pool_base;

ArrayListPairs colliding_pairs;

struct poolBall pool_balls_array[NUMBER_OF_POOL_BALLS];
struct pocketHole pool_pockets_array[NUMBER_OF_POCKETS];

struct ButtonKeys Keys = {0};
struct MouseData Mouse = {0};
struct RackPositions Rack = {0};

struct poolBall *selected_ball = NULL;

GLTtext *text = NULL;

// Main game functions
void init();
void cleanup();
void resize(int w, int h);
void display();
void draw_game(float deltaTime);

// Ball and pocket management
void place_balls();
void place_pockets();
void reset();
void ball_selected();

// Debugging / printing
void print_ball_data(struct poolBall *ball);
void print_pocket_data(struct pocketHole *pocket);

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(GAME_WINDOW_WIDTH, GAME_WINDOW_HEIGHT);
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
    glutInitWindowPosition(200, 200);
    glutCreateWindow("Billards - V1.0");

    init();
    glutDisplayFunc(display);
    glutReshapeFunc(resize);
    glutMouseFunc(mouse_button_handler);
    glutPassiveMotionFunc(mouse_movement);
    glutMotionFunc(mouse_movement);
    glutKeyboardFunc(ButtonDown);
    glutKeyboardUpFunc(ButtonUp);
    glutMainLoop();

    return 0;
}

void init() {
    printf("Initializing...\n");
    glClearColor(0.3, 0.3, 0.3, 1.0f);
    gluOrtho2D(0, GAME_WINDOW_WIDTH, GAME_WINDOW_HEIGHT, 0);
    
    init_arraylist(&colliding_pairs, ARRAYLIST_CAPACITY);

    glewInit();
    // gltInit();

    // if(gltInitialized == FALSE){
    //     exit(EXIT_FAILURE);
    // }

    // text = gltCreateText();

    place_balls();
    place_pockets();

    texture_background = load_texture("textures/floor.png");
    texture_pool_table = load_texture("textures/table.png");
    texture_pool_cue = load_texture("textures/poolcue.png");
    texture_pool_rack = load_texture("textures/poolrack.png");
    texture_pool_base = load_texture("textures/basetable.png");
}

void cleanup(){
    free_arraylist(&colliding_pairs);

    if(text){
        // gltDeleteText(text);
        text = NULL;
    }

    // gltTerminate();
    return;
}

void resize(int w, int h) {
    glutReshapeWindow(GAME_WINDOW_WIDTH,GAME_WINDOW_HEIGHT);
}

void display() {
    //fps
    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    float deltaTime = (currentTime - frame1) / 1000.0f;
    frame1 = currentTime;

    glutPostRedisplay();
    glClear(GL_COLOR_BUFFER_BIT);

    draw_game(deltaTime);

    glutSwapBuffers();
}

void draw_game(float deltaTime){
    draw_background();

    for (int i = 0; i < NUMBER_OF_POCKETS; i++) {
        if (pool_pockets_array[i].visible == TRUE) {
            struct pocketHole *curr_pocket = &pool_pockets_array[i];
            draw_pockets(CIRCLE_DEGREES, curr_pocket);
        }
    }

    ball_selected();

    if(check_collision(deltaTime) == RESET)
        reset();

    for(int i = 0; i < NUMBER_OF_POOL_BALLS; i++){
        if(pool_balls_array[i].visible == TRUE){
            struct poolBall *curr_ball = &pool_balls_array[i];
            //printf("current velocity total [%d] = %f\n", curr_ball->id, curr_ball->vel_x * curr_ball->vel_x + curr_ball->vel_y * curr_ball->vel_y);
            fflush(stdout);
            draw_pool_ball(CIRCLE_DEGREES, curr_ball);
        }
    }

    while(colliding_pairs.size > 0) {
        // draw_line(
        //     (Point){.x = colliding_pairs.data->first->pos_x, .y = colliding_pairs.data->first->pos_y},
        //     (Point){.x = colliding_pairs.data->second->pos_x, .y = colliding_pairs.data->second->pos_y}
        // );
        //printf("(%d) vs (%d)\n", colliding_pairs.data->first->id, colliding_pairs.data->second->id);
        erase(&colliding_pairs, 0);
    }

    if(Keys.a == 1){
        reset();
    }

    if(Keys.esc == 1){
        printf("ecs IS PRESSED\n");
        cleanup();
        glutLeaveMainLoop();
    }

    if(Mouse.mouse_pressed == TRUE && selected_ball != NULL){
        draw_pool_cue(texture_pool_cue);
    }

    if(Mouse.mouse_pressed == TRUE){
        draw_line(
            (Point){.x = Mouse.pressed_mouse_x, .y = Mouse.pressed_mouse_y},
            (Point){.x = Mouse.curr_mouse_x, .y = Mouse.pressed_mouse_y}
        );

        draw_line(
            (Point){.x = Mouse.pressed_mouse_x, .y = Mouse.pressed_mouse_y},
            (Point){.x = Mouse.pressed_mouse_x, .y = Mouse.curr_mouse_y}
        );

        draw_line(
            (Point){.x = Mouse.curr_mouse_x, .y = Mouse.curr_mouse_y},
            (Point){.x = Mouse.pressed_mouse_x, .y = Mouse.curr_mouse_y}
        );

        draw_line(
            (Point){.x = Mouse.curr_mouse_x, .y = Mouse.curr_mouse_y},
            (Point){.x = Mouse.curr_mouse_x, .y = Mouse.pressed_mouse_y}
        );
    }
}

void place_pockets(){
    printf("Placing Pockets\n");
    int pocket_x = 0;
    int pocket_y = 0;
    
    for(int i = 0; i < NUMBER_OF_POCKETS; i++){
        pool_pockets_array[i].radius = POCKET_RADIUS;
        pool_pockets_array[i].id = i;
        pool_pockets_array[i].visible = TRUE;

        switch(pool_pockets_array[i].id){
            case 0:
                pocket_x = POOL_TABLE_X;
                pocket_y = POOL_TABLE_Y;
                break;
            case 1:
                pocket_x = POOL_TABLE_X + POOL_TABLE_WIDTH;
                pocket_y = POOL_TABLE_Y;
                break;
            case 2:
                pocket_x = POOL_TABLE_X + POOL_TABLE_WIDTH;
                pocket_y = POOL_TABLE_Y + POOL_TABLE_HEIGHT;
                break;
            case 3:
                pocket_x = POOL_TABLE_X;
                pocket_y = POOL_TABLE_Y + POOL_TABLE_HEIGHT;
                break; 
            case 4:
                pocket_x = POOL_TABLE_X + POOL_TABLE_WIDTH;
                pocket_y = POOL_TABLE_Y + (POOL_TABLE_HEIGHT / 2);
                break;  
            case 5: 
                pocket_x = POOL_TABLE_X;
                pocket_y = POOL_TABLE_Y + (POOL_TABLE_HEIGHT / 2);

                pocket_x = GAME_WINDOW_WIDTH / 2;
                pocket_y = GAME_WINDOW_HEIGHT / 2;
                break;  
            default:
                printf("ERROR IN PLACING POCKETS\n");
        }
        
        pool_pockets_array[i].pos_x = pocket_x;
        pool_pockets_array[i].pos_y = pocket_y;
        print_pocket_data(&pool_pockets_array[i]);

        pocket_x = 0;
        pocket_y = 0;
    }
}

void place_balls(){
    printf("Placing Balls\n");
    int i;
    int ball_index = 1; // after cue ball
    float rack_start_x = GAME_WINDOW_WIDTH * 0.50f;   // rack position on table
    float rack_start_y = GAME_WINDOW_HEIGHT * 0.30f;
    srand(time(NULL));

    for(i = 0; i < NUMBER_OF_POOL_BALLS; i++){
        pool_balls_array[i].radius = CIRCLE_RADIUS;
        pool_balls_array[i].number = i;
        pool_balls_array[i].striped = (i >= 9) ? TRUE : FALSE;
        if(i == 0){
            pool_balls_array[i].color = 0;
        }
        else{
            pool_balls_array[i].color = (i % 15) + 1; // i is an index into the pool_ball_colors array
        }
        pool_balls_array[i].visible = TRUE;
        pool_balls_array[i].id = i;
        pool_balls_array[i].mass = random_float_range((CIRCLE_RADIUS - 1) * 5.5, (CIRCLE_RADIUS - 1) * 5.5);
        pool_balls_array[i].vel_x = 0;
        pool_balls_array[i].vel_y = 0;
        pool_balls_array[i].acc_x = 0;
        pool_balls_array[i].acc_y = 0;

        //print_ball_data(&pool_balls_array[i]);
    }

    pool_balls_array[CUE_BALL].pos_x = GAME_WINDOW_WIDTH * 0.5f;
    pool_balls_array[CUE_BALL].pos_y = GAME_WINDOW_HEIGHT * 0.75f;

    int max_rack_balls = NUMBER_OF_POOL_BALLS - 1;

    printf("Selecting Positions\n");
    printf("Rack Rows: %d\n", NUMBER_RACK_ROWS);

    for (int row = 0; row < NUMBER_RACK_ROWS && ball_index <= max_rack_balls; row++) {
        for (int col = 0; col <= row && ball_index <= max_rack_balls; col++) {
            pool_balls_array[ball_index].pos_x = rack_start_x + SPACING + (col - row * 0.5f) * (BALL_SPACING * 1.1f);
            pool_balls_array[ball_index].pos_y = rack_start_y - row * BALL_SPACING * 1.0f;

            ball_index++;
        }
    }
}

void reset(){
    printf("Reseting...\n");
    printf("score was: %d\n", score);
    for(int i = 0; i < colliding_pairs.size; i++){
        erase(&colliding_pairs, 0);
    }
    
    score = 0;
    place_balls();
    place_pockets();
}

void ball_selected(){
    // if(selected_ball == NULL){
    //     printf("SELECTED BALL: NULL\n");
    // }else{
    //     printf("SELECTED BALL: %d\n", selected_ball->number);
    // }
    fflush(stdout);

    if(selected_ball != NULL && Mouse.mouse_pressed == TRUE){
        return;
    }

    for(int i = 0; i < NUMBER_OF_POOL_BALLS; i++){ //balls should not be moving
        struct poolBall *curr_ball = &pool_balls_array[i];
        if(curr_ball->vel_x != 0 || curr_ball->vel_y != 0){
            return;
        }
    }

    if(can_selection_printed == TRUE){
        printf("you may select\n");
        fflush(stdout);
        can_selection_printed = FALSE;
    }

    if(Mouse.mouse_pressed == FALSE){
        if(selected_ball != NULL){
            selected_ball->vel_x = 5.0f * ((selected_ball->pos_x) - (float)Mouse.curr_mouse_x);
            selected_ball->vel_y = 5.0f * ((selected_ball->pos_y) - (float)Mouse.curr_mouse_y);
            can_selection_printed = TRUE;
        }
        selected_ball = NULL;
    }

    if(Mouse.mouse_pressed == TRUE){
        if(is_point_in_circle(pool_balls_array[CUE_BALL].pos_x, pool_balls_array[CUE_BALL].pos_y, pool_balls_array[CUE_BALL].radius, Mouse.curr_mouse_x, Mouse.curr_mouse_y) == TRUE && pool_balls_array[CUE_BALL].id == 0){ //only the cue ball
            selected_ball = &pool_balls_array[CUE_BALL];
        }
    }
}

void print_ball_data(struct poolBall *ball){
    printf("Printing Ball Data\n-------------------------------\n");
    printf("Radius: %.2f\n", ball->radius);
    printf("Number: %d", ball->number);
    printf("Striped: %s\n", ball->striped ? "TRUE" : "FALSE");
    printf("Color:");
    for(int i = 0; i < 3; i++){
        printf(" %.2f", pool_ball_colors[ball->color][i]);
    }
    printf("\n");
    printf("Visible: %s\n", ball->visible ? "TRUE" : "FALSE");
    printf("Id: %d\n", ball->id);
    printf("Mass : %f\n", ball->mass);
    printf("Velocity: %.2f %.2f\n", ball->vel_x, ball->vel_y);
    printf("Accleration: %.2f %.2f\n", ball->acc_x, ball->acc_y);
    printf("--------------------------------\n");
}

void print_pocket_data(struct pocketHole *pocket){
    printf("Printing Pocket Data\n-------------------------------------\n");
    printf("Radius: %.2f\n", pocket->radius);
    printf("Visible: %s\n", pocket->visible ? "TRUE" : "FALSE");
    printf("Id: %d\n", pocket->id);
    printf("Position: %.2f %.2f\n", pocket->pos_x, pocket->pos_y);
    printf("-------------------------------------\n");
}

