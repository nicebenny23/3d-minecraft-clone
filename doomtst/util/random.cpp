#pragma once
#include "random.h"


uint64_t seed64 = 0;
namespace random {
	inline void randomize_64(uint64_t& seed_64) {

		seed64 ^= seed64 << 13;
		seed64 ^= seed64 >> 7;
		seed64 ^= seed64 << 17;

	}
	//random number from 0-1
	float random() {
		randomize_64(seed64);
		return static_cast<double>(seed64) / UINT64_MAX;
	}
	bool randombool() {
		randomize_64(seed64);
		return seed64 % 2;
	}

	void randomize_uint(unsigned int& seed) {
		seed ^= seed << 13;
		seed ^= seed >> 17;
		seed ^= seed << 5;
	}

	float random(float max) {

		return random() * max;
	}
	stn::array<v3::Vec3> seeded_directions;

	void init_random_direction_cache() {
		
		const int iters = 10;
		unsigned int noiseval = 1;

		seeded_directions = stn::array<v3::Vec3>();
		size_t ushort_amt = (1 + std::numeric_limits<unsigned short>().max());
		for (int u = 0; u < ushort_amt; u++) {

			v3::Vec3 point_on_circle;
			do {

				for (int ind = 0; ind < 3; ind++) {

					for (size_t i = 0; i < iters; i++) {
						randomize_uint(noiseval);
					}
					point_on_circle[ind] = (noiseval/ static_cast<double>(MAXUINT32))*2-1;
				}

			} while (mag2(point_on_circle) > 1);
			seeded_directions.push(normal(point_on_circle));
		}
	}
	

	v3::Vec3 spherical() {
		randomize_64(seed64);
		return seeded_directions.unchecked_at(seed64&std::numeric_limits<unsigned short>().max());
	}

	void initilize_random() {
		init_random_direction_cache();
		seed64 = 1;
	}
}