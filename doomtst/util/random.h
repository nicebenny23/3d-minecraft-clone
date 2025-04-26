#include <limits.h>
#include <stdint.h>
#include <math.h>
#include "vector3.h"
#include <time.h>
#include "dynamicarray.h"

#pragma once

float random();
float random(float max);
bool randombool();
unsigned int randomint();
void randomcoord(unsigned int& seed);

int randomint( int max);

extern dynamicarray::array<v3::Vector3> seededdirections;
int Hash(int seed, int xPrimed, int yPrimed, int zPrimed);
inline v3::Vector3 randompointonsphere(int x, int y, int z) {
	int seed = 3;
	int hash = Hash(seed, x, y, z);
	hash ^= hash >> 15;
	hash &= MAXSHORT - 1;
	return seededdirections.UncheckedAt(hash);

}

void initrandom();
