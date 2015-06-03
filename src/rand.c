#include <pebble.h>
#include "rand.h"

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
int rand_curseed = 132;

void
rand_seed()
{
    rand_curseed = time(0);
}

int
rand_choice(int nchoice)
{
    if (nchoice < 0)
	return -rand_choice(-nchoice);
    if (nchoice < 2)
	return 0;

    u32		val = RAND();

    // Wang int-hash.
    val = (val ^ 61) ^ (val >> 16);
    val = val + (val << 3);
    val = val ^ (val >> 4);
    val = val * 0x27d4eb2d;
    val = val ^ (val >> 15);

    // eat bad bits and avoid sign
    val >>= 1;

    return val % nchoice;
}

void
rand_angletodir(int angle, int *dx, int *dy)
{
    if (angle & 2)
	*dx = *dy = -1;
    else
	*dx = *dy = 1;

    if (angle & 1)
	*dx = 0;
    else
	*dy = 0;
}

void
rand_dir(int *dx, int *dy)
{
    u32		val = RAND();

    // Mix in the full entropy.
    val ^= val >> 16;
    val ^= val >> 8;
    val ^= val >> 4;
    val ^= val >> 2;

    rand_angletodir(val, dx, dy);
}

