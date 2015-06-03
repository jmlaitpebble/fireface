#pragma once
	
#include "mytypes.h"
//
// Defines the rand functions.
// RAM use: 4 bytes
//

//
// Prototypes
//
void rand_seed();
int rand_choice(int nchoice);
void rand_angletodir(int angle, int *dx, int *dy);
void rand_dir(int *dx, int *dy);

//
// implementation
//
extern int rand_curseed;

#define RAND() (rand_curseed = (rand_curseed * 1013904223 + 1664525))
#define FORALL_4DIR(dx, dy) \
    for (int lcl_angle = 0; rand_angletodir(lcl_angle, &dx, &dy), lcl_angle < 4; lcl_angle++)


