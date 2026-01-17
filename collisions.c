#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "pool.h"
#include "arraylist.h"
#include "collisions.h"

int is_point_in_circle(float x1, float y1, float r1, int px, int py){
    return fabs((x1 - px) * (x1 - px) + (y1 - (float) py) * (y1 - (float) py)) <= (r1 * r1);
}

int do_poolballs_overlap(float x1, float y1, float r1, float x2, float y2, float r2){
    return fabs((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)) <= (r1 + r2) * (r1 + r2);
}

int do_pocket_and_ball_overlap(float x1, float y1, float r1, float x2, float y2, float r2){
    return fabs((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)) <= (r1 + r2) * (r1 + r2);
}

int check_collision(float elapsed_time){
    int status = 0;
    colliding_pairs.size = 0;

    if (colliding_pairs.size > colliding_pairs.capacity) {
        printf("COLLISION OVERFLOW! %ld > %ld\n", colliding_pairs.size, colliding_pairs.capacity);
        fflush(stdout);
    }

    if((status = update_ball_data(elapsed_time)) == RESET)
        return RESET;
    
    if((status = check_pocket_collision()) == RESET)
        return RESET;
    
    if((check_collisions_between_balls_wall()) == RESET)
        return RESET;
    
    if((check_dyanmic_collisions()) == RESET)
        return RESET;
}

int update_ball_data(float delta_time){
    for(int i = 0; i < NUMBER_OF_POOL_BALLS; i++){
        struct poolBall *curr_ball = &pool_balls_array[i];
        curr_ball->acc_x = -curr_ball->vel_x * FRICTION;
        curr_ball->acc_y = -curr_ball->vel_y * FRICTION;        
        curr_ball->vel_x += curr_ball->acc_x * delta_time;
        curr_ball->vel_y += curr_ball->acc_y * delta_time;
        curr_ball->pos_x += curr_ball->vel_x * delta_time;
        curr_ball->pos_y += curr_ball->vel_y * delta_time;

        // printf("BALL: acc(%f, %f), vel(%f, %f), pos(%f, %f)\n", curr_ball->acc_x,
        //                                                         curr_ball->acc_y,
        //                                                         curr_ball->vel_x,
        //                                                         curr_ball->vel_y,
        //                                                         curr_ball->pos_x,
        //                                                         curr_ball->pos_y
        // );

        if(fabs(curr_ball->vel_x * curr_ball->vel_x + curr_ball->vel_y * curr_ball->vel_y) < CLAMP_SPEED){
            curr_ball->vel_x = 0;
            curr_ball->vel_y = 0;
        }
    }

    return 0;
}

int check_pocket_collision(){
    for(int i = 0; i < NUMBER_OF_POOL_BALLS; i++){
        struct poolBall *curr_ball = &pool_balls_array[i];
        for(int j = 0; j < NUMBER_OF_POCKETS; j++){
            struct pocketHole *curr_pocket = &pool_pockets_array[j];
            if(do_pocket_and_ball_overlap(curr_ball->pos_x, curr_ball->pos_y, curr_ball->radius, curr_pocket->pos_x, curr_pocket->pos_y, curr_pocket->radius / 2) == TRUE && curr_ball->visible != FALSE){
                curr_ball->visible = FALSE;
                curr_ball->acc_x = 0;
                curr_ball->acc_y = 0;
                curr_ball->vel_x = 0;
                curr_ball->vel_y = 0;
                curr_ball->pos_x = 0;
                curr_ball->pos_y = 0;

                if(curr_ball->id == CUE_BALL){
                    printf("you messed up\n");
                    return RESET;
                }

                printf("Ball (%d) was sunk\n", curr_ball->number);
                score += curr_ball->number;
                printf("Score: %d\n", score);
                fflush(stdout);
            }
        }
    }

    return 0;
}

int check_collisions_between_balls_wall(){
    for(int step = 0; step <= 2; step++){
        for(int i = 0; i < NUMBER_OF_POOL_BALLS; i++){
            struct poolBall *curr_ball = &pool_balls_array[i];
            for(int j = i + 1; j < NUMBER_OF_POOL_BALLS; j++){
               struct poolBall *target_ball = &pool_balls_array[j];

                if(do_poolballs_overlap(curr_ball->pos_x, curr_ball->pos_y, curr_ball->radius, 
                                        target_ball->pos_x, target_ball->pos_y, target_ball->radius ) 
                                        == TRUE && (curr_ball->id != target_ball->id && (curr_ball->visible) == TRUE)){

                    struct poolBallPair collided;
                    collided.first = &pool_balls_array[i];
                    collided.second = &pool_balls_array[j];
                    push_back(&colliding_pairs, collided);

                    // distnce between ball centers
                    float center_distance = sqrtf(
                    (curr_ball->pos_x - target_ball->pos_x) * (curr_ball->pos_x - target_ball->pos_x) + 
                    (curr_ball->pos_y - target_ball->pos_y) * (curr_ball->pos_y - target_ball->pos_y));   
                    
                    float overlap = 0.5 * (center_distance - curr_ball->radius - target_ball->radius);

                    curr_ball->pos_x -= overlap * (curr_ball->pos_x - target_ball->pos_x) / center_distance;
                    curr_ball->pos_y -= overlap * (curr_ball->pos_y - target_ball->pos_y) / center_distance;

                    target_ball->pos_x += overlap * (curr_ball->pos_x - target_ball->pos_x) / center_distance;
                    target_ball->pos_y += overlap * (curr_ball->pos_y - target_ball->pos_y) / center_distance; 
                }
            }

            // Right wall
            if(curr_ball->pos_x >= (POOL_TABLE_WIDTH + POOL_TABLE_X) - curr_ball->radius) {
                curr_ball->pos_x = (POOL_TABLE_WIDTH + POOL_TABLE_X) - curr_ball->radius; 
                curr_ball->vel_x = -fabs(curr_ball->vel_x);                 // invert X velocity
            }

            // Left wall
            if(curr_ball->pos_x <= POOL_TABLE_X + curr_ball->radius) {
                curr_ball->pos_x = POOL_TABLE_X + curr_ball->radius;
                curr_ball->vel_x = fabs(curr_ball->vel_x);                  // invert X velocity
            }

            // Bottom wall
            if(curr_ball->pos_y >= (POOL_TABLE_HEIGHT + POOL_TABLE_Y) - curr_ball->radius) {
                curr_ball->pos_y = (POOL_TABLE_HEIGHT + POOL_TABLE_Y) - curr_ball->radius;
                curr_ball->vel_y = -fabs(curr_ball->vel_y);                // invert Y velocity
            }

            // Top wall
            if(curr_ball->pos_y <= POOL_TABLE_Y + curr_ball->radius) {
                curr_ball->pos_y = POOL_TABLE_Y + curr_ball->radius;
                curr_ball->vel_y = fabs(curr_ball->vel_y);                 // invert Y velocity
            }
        }
    }
    
    return 0;
}

int check_dyanmic_collisions(){
    for(int i = 0; i < colliding_pairs.size; i++){
        struct poolBall *ball1 = colliding_pairs.data[i].first;
        struct poolBall *ball2 = colliding_pairs.data[i].second;

        float center_distance = sqrtf(
                    (ball1->pos_x - ball2->pos_x) * (ball1->pos_x - ball2->pos_x) + 
                    (ball1->pos_y - ball2->pos_y) * (ball1->pos_y - ball2->pos_y));   
        
        //normals
        float normal_x = (ball2->pos_x - ball1->pos_x) / center_distance;
        float normal_y = (ball2->pos_y - ball1->pos_y) / center_distance;

        //tangents
        float tan_x = -normal_y;
        float tan_y = normal_x;
            
        //Dot product tang
        float dp_tan_b1 = ball1->vel_x * tan_x + ball1->vel_y * tan_y;
        float dp_tan_b2 = ball2->vel_x * tan_x + ball2->vel_y * tan_y;

        //Dot product normal
        float dp_norm_b1 = ball1->vel_x * normal_x + ball1->vel_y * normal_y;
        float dp_norm_b2 = ball2->vel_x * normal_x + ball2->vel_y * normal_y;

        //convervation of momentum in 1d
        float m1 = (dp_norm_b1 * (ball1->mass - ball2->mass) + 2.0f * ball2->mass * dp_norm_b2) / (ball1->mass + ball2->mass);
        float m2 = (dp_norm_b2 * (ball2->mass - ball1->mass) + 2.0f * ball1->mass * dp_norm_b1) / (ball1->mass + ball2->mass);
        
        ball1->vel_x = tan_x * dp_tan_b1 + normal_x * m1;
        ball1->vel_y = tan_y * dp_tan_b1 + normal_y * m1;
        ball2->vel_x = tan_x * dp_tan_b2 + normal_x * m2;
        ball2->vel_y = tan_y * dp_tan_b2 + normal_y * m2;
    }

    return 0;
}
