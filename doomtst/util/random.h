#include <limits.h>
#include <stdint.h>
#include <math.h>
#include "vector3.h"
#include <time.h>
#ifndef random_hpp
#define random_hpp

float random();
float random(float max);
bool randombool();
unsigned int randomint();
void randomcoord(uint64_t& seed);
v3::Vector3 randomseeded(uint64_t x, uint64_t y, uint64_t z);
int random(int val, int seed);
bool randombool(float truechange);
int randomint(float max);



void randominit();

#endif // !random_hpp#pragma once
