#include <limits.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#ifndef random_hpp
#define random_hpp

float random();
float random(float max);
bool randombool();
bool randombool(float truechange);
int randomint(float max);



void randominit();

#endif // !random_hpp#pragma once
