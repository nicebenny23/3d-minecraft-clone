
#include <stdint.h>
#include <math.h>
#include "../math/vector3.h"
#include "dynamicarray.h"

#pragma once
namespace random {

	//random number from [0,1]
	float random();
	//random number from [0,max]
	float random(float max);

	bool randombool();
	void randomize_uint(unsigned int& seed);
	extern stn::array<v3::Vec3> seeded_directions_sphere;

	extern stn::array<v3::Vec3> seeded_directions_cube;
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
		return seeded_directions_sphere[hash_coord(seed, x, y, z)];

	}
	inline v3::Vec3 cubical(int x, int y, int z, size_t seed) {
  		return seeded_directions_cube[hash_coord(seed, x, y, z)];

	}
	void initilize_random();

}
