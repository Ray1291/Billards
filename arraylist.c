#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "pool.h"

void init_arraylist(ArrayListPairs *v, size_t initial_capacity){
    v->data = (struct poolBallPair *) malloc(initial_capacity * sizeof(struct poolBallPair));
    if(v->data == NULL){
        printf("FAILED TO INIT ARRAY\n");
        fflush(stdout);
    }
    v->size = 0;
    v->capacity = initial_capacity;
}

void push_back(ArrayListPairs *v, struct poolBallPair new_pair){
    //vector full -> increased size
    if(v->size >= v->capacity){
        v->capacity *= 2;
        v->data = (struct poolBallPair *) realloc(v->data, v->capacity * sizeof(struct poolBallPair));
        if(v->data == NULL){
            printf("FAILED TO INCREASE PAIR ARRAY SIZE\n");
            exit(EXIT_FAILURE);
        }
    }

    v->data[v->size++] = new_pair;
}

void erase(ArrayListPairs *v, size_t position){
    //printf("ERASING ELEMENT POS: %ld\n", position);
    if(v == NULL || v->size == 0){
        printf("arraylist is empty\n");
        return;
    }
    //vector is empty
    if(position < 0 || position >= v->size){
        printf("errer: invalid erase position\n");
        return;
    }

    // free(v->data[position].first);
    // free(v->data[position].second);

    v->data[position] = v->data[v->size - 1];
    v->data[v->size - 1] = (struct poolBallPair){ NULL, NULL };
    v->size--;
}

void free_arraylist(ArrayListPairs *v){
    printf("FREEING LIST\n");
    free(v->data);
    v->size = 0;
    v->capacity = 0;
}
