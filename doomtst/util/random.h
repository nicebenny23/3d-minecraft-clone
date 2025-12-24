
#include <stdint.h>
#include <math.h>
#include "../math/vector3.h"
#include "dynamicarray.h"

#pragma once
namespace random {
	
	float random();
	float random(float max);

	bool randombool();
	void randomcoord(unsigned int& seed);
	extern stn::array<v3::Vec3> seededdirections;

int Hash(int seed, int xPrimed, int yPrimed, int zPrimed);

	inline v3::Vec3 spherical(int x, int y, int z) {
		int seed = 3;
		int hash = Hash(seed, x, y, z);
		hash ^= hash >> 15;
		hash &= MAXSHORT - 1;
		return seededdirections.unchecked_at(hash);

	}
	void initrandom();

}
