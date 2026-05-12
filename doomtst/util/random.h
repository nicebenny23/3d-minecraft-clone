
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
	extern stn::array<v3::Vec3> seeded_directions;
	inline unsigned short hash_coord(int seed, int xprimed, int yprimed, int zprimed) {
		unsigned int hash = seed ^ xprimed ^ yprimed ^ zprimed;
		hash *= 0x27d4eb2d;
		hash ^= hash >> 15;
		hash *= 0x27d4eb2d;
		hash ^= hash >> 15;
		hash *= 0x27d4eb2d;
		hash ^= hash >> 15;
		hash *= 0x27d4eb2d;
		hash ^= hash >> 15;
		hash *= 0x27d4eb2d;
		hash ^= hash >> 15;
		return hash;
	}
	v3::Vec3 spherical();
	inline v3::Vec3 spherical(int x, int y, int z, size_t seed) {
		return seeded_directions[hash_coord(seed, x, y, z)];

	}
	void initilize_random();

}
