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
void randomcoord(unsigned int& seed);
v3::Vector3 randomseeded(unsigned int x, unsigned int y, unsigned int z);
int random(int val, int seed);
bool randombool(float truechange);
int randomint( int max);



void randominit();

#endif // !random_hpp#pragma once
