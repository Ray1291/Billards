#ifndef ARRAYLIST_H
#define ARRAYLIST_H

#include "pool.h"

void print_ball_data(struct poolBall *);
void init_arraylist(ArrayListPairs *, size_t);
void push_back(ArrayListPairs *, struct poolBallPair );
void erase(ArrayListPairs *v, size_t position);
void free_arraylist(ArrayListPairs *);

#endif