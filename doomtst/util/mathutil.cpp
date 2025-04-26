#include "mathutil.h"
#pragma once

int comparefloat(const void* b, const void* a)
{
	return sign(*((float*)a) - *((float*)b));
}
