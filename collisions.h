#ifndef COLLISION_H
#define COLLISION_H

#define RESET 127

//Point Checking
int is_point_in_circle(float, float, float, int, int);
int do_poolballs_overlap(float, float, float, float, float, float);
int do_pocket_and_ball_overlap(float, float, float, float, float, float);

// Colision Handling
int check_collision(float elapsed_time);
int update_ball_data(float delta_time);
int check_pocket_collision();
int check_collisions_between_balls_wall();
int check_dyanmic_collisions();

#endif 